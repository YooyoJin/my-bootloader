/**
 * @file    iapupdate.c
 * @brief   IAP升级核心业务实现
 * @note    移植自华大bootloader工程bootloader_iap.c: 判定流程/校验算法/外部Flash布局不变;
 *          CRC改用本仓库modules/crc(Crc16ModbusStep流式, 与原工程高低字节双表法数学等价);
 *          原工程MDK版在0x2000放置ID码常量, GCC链接未生效, 此处以宏IAP_IMAGE_ID_CODE等价实现。
 */
#include <stdbool.h>
#include <stdint.h>
#include "iapupdate.h"
#include "bspexflash.h"
#include "crc.h"
#include "jumpapp.h"
#include "log.h"
#include "hc32_ll.h"

// 内部Flash单扇区搬运缓冲(8KB)
static uint8_t g_au8SectorBuf[IAP_INNER_FLASH_SECTOR_SIZE];
// CRC流式校验的临时读块缓冲
static uint8_t g_au8CrcChunk[IAP_CRC_CHUNK_SIZE];

/** @brief 升级头的3字节大端字段组装为24位无符号数 */
static uint32_t IapWord24FromBe(const uint8_t au8Bytes[3])
{
    return ((uint32_t)au8Bytes[0] << 16) | ((uint32_t)au8Bytes[1] << 8) | (uint32_t)au8Bytes[2];
}

/** @brief 读完成标志: 已置位说明上次搬运已完成, 无需再升级 */
static bool IapIsUpdateDone(void)
{
    uint8_t au8Flag[2];

    BspExFlashRead(IAP_EXFLASH_DONE_FLAG_ADDR, au8Flag, sizeof(au8Flag));

    return (IAP_FLAG_DONE_BYTE0 == au8Flag[0]) && (IAP_FLAG_DONE_BYTE1 == au8Flag[1]);
}

/** @brief 读请求标志: 已置位说明App侧已把新固件镜像完整存入外部Flash */
static bool IapIsUpdateRequested(void)
{
    uint8_t au8Flag[2];

    BspExFlashRead(IAP_EXFLASH_REQ_FLAG_ADDR, au8Flag, sizeof(au8Flag));

    return (IAP_FLAG_REQ_BYTE0 == au8Flag[0]) && (IAP_FLAG_REQ_BYTE1 == au8Flag[1]);
}

/**
 * @brief  校验镜像有效性: 签名码 + 长度上限 + 全流CRC16
 * @param  pstcHead       升级头(外部Flash前16B)
 * @param  pu32ImageSize  出参, 镜像字节数
 * @retval true 校验通过
 * @note   CRC流: 升级头前9B → 地址9起的"预留7B+镜像+尾部2B CRC", 共Size+9+2字节;
 *         尾部2B是使全流余数为0的CRC值, 故最终CRC寄存器为0即校验通过
 */
static bool IapVerifyImage(const stcIapImageHead_t *pstcHead, uint32_t *pu32ImageSize)
{
    uint16_t u16Crc = 0xFFFFu;  // CRC-16/MODBUS初值
    uint32_t u32Addr;
    uint32_t u32Remain;
    uint32_t u32Chunk;
    uint32_t u32i;

    *pu32ImageSize = IapWord24FromBe(pstcHead->au8ImageSize);

    if (IapWord24FromBe(pstcHead->au8Signature) != IAP_IMAGE_ID_CODE)
    {
        LOG_WARN("IAP: signature NG");
        return false;  // 签名码不吻合, 忽略本次镜像
    }

    if (*pu32ImageSize > IAP_IMAGE_MAX_SIZE)
    {
        LOG_WARN("IAP: size %u NG", *pu32ImageSize);
        return false;  // 镜像超长, 无效
    }

    // 1) CRC喂入升级头前9字节(长度3B+起始地址3B+签名码3B)
    for (u32i = 0u; u32i < 9u; u32i++)
    {
        Crc16ModbusStep(((const uint8_t *)pstcHead)[u32i], &u16Crc);
    }

    // 2) CRC喂入地址9起的剩余流: 预留7B(地址9~15)+镜像+尾部2B CRC
    u32Addr    = IAP_EXFLASH_HEAD_ADDR + 9u;
    u32Remain  = *pu32ImageSize + 2u + 7u;
    while (u32Remain != 0u)
    {
        u32Chunk = (u32Remain < IAP_CRC_CHUNK_SIZE) ? u32Remain : IAP_CRC_CHUNK_SIZE;
        BspExFlashRead(u32Addr, g_au8CrcChunk, u32Chunk);
        for (u32i = 0u; u32i < u32Chunk; u32i++)
        {
            Crc16ModbusStep(g_au8CrcChunk[u32i], &u16Crc);
        }
        u32Addr += u32Chunk;
        u32Remain -= u32Chunk;
    }

    return (0u == u16Crc);  // 全流余数为0即CRC通过
}

/**
 * @brief  编程内部Flash一个扇区: 解锁→擦除→编程→回锁
 * @param  u32Addr 内部Flash扇区起始地址(8KB对齐)
 * @param  pu8Buf  数据源
 * @param  u32Len  字节数(不超过一个扇区)
 */
static void IapProgramInnerSector(uint32_t u32Addr, const uint8_t *pu8Buf, uint32_t u32Len)
{
    LL_PERIPH_WE(LL_PERIPH_EFM);  // 解锁EFM写保护
    EFM_FWMC_Cmd(ENABLE);         // 允许Flash写操作
    (void)EFM_SectorErase(u32Addr);
    (void)EFM_Program(u32Addr, (void *)(uintptr_t)pu8Buf, u32Len);  // SDK形参无const, 显式去const
    EFM_FWMC_Cmd(DISABLE);        // 禁止Flash写操作
    LL_PERIPH_WP(LL_PERIPH_EFM);  // 回锁EFM写保护
}

/**
 * @brief  把外部Flash镜像搬运到内部Flash APP区(自0x4000起, 8KB扇区为单位)
 * @param  u32ImageSize 镜像字节数(已通过校验)
 * @note   尾部不足一扇区的残余块单独擦写
 */
static bool IapProgramAppArea(uint32_t u32ImageSize)
{
    uint32_t u32SrcAddr = IAP_EXFLASH_IMAGE_START_ADDR;
    uint32_t u32DstAddr = BOOT_APP_START_ADDR;

    while (SET != EFM_GetStatus(EFM_FLAG_RDY))
    {
        ;  // 等待EFM就绪
    }

    while (u32ImageSize >= IAP_INNER_FLASH_SECTOR_SIZE)
    {
        BspExFlashRead(u32SrcAddr, g_au8SectorBuf, IAP_INNER_FLASH_SECTOR_SIZE);
        IapProgramInnerSector(u32DstAddr, g_au8SectorBuf, IAP_INNER_FLASH_SECTOR_SIZE);
        u32SrcAddr += IAP_INNER_FLASH_SECTOR_SIZE;
        u32DstAddr += IAP_INNER_FLASH_SECTOR_SIZE;
        u32ImageSize -= IAP_INNER_FLASH_SECTOR_SIZE;
    }

    if (u32ImageSize != 0u)  // 尾部不足一扇区的残余块
    {
        BspExFlashRead(u32SrcAddr, g_au8SectorBuf, u32ImageSize);
        IapProgramInnerSector(u32DstAddr, g_au8SectorBuf, u32ImageSize);
    }

    // todo 与原工程一致: 未做编程后回读比对, 擦/写返回值也未判定
    return true;
}

/** @brief 清除外部Flash两个标志扇区(完成+请求) */
static void IapClearUpdateFlags(void)
{
    BspExFlashSectorErase(IAP_EXFLASH_DONE_FLAG_ADDR);
    BspExFlashSectorErase(IAP_EXFLASH_REQ_FLAG_ADDR);
}

/** @brief 写升级完成标志(0x36 0x63)并等待外部Flash写入完成 */
static void IapMarkUpdateDone(void)
{
    uint8_t au8Flag[2] = {IAP_FLAG_DONE_BYTE0, IAP_FLAG_DONE_BYTE1};

    BspExFlashWrite(IAP_EXFLASH_DONE_FLAG_ADDR, au8Flag, sizeof(au8Flag));
    (void)BspExFlashWaitIdle(10000u);
}

/**
 * @brief  IAP升级入口: 检查标志位决定是否搬运镜像(流程对齐原工程iap_update)
 */
void IapUpdate(void)
{
    stcIapImageHead_t stcHead;
    uint32_t          u32ImageSize;

    if (IapIsUpdateDone())
    {
        LOG_INFO("IAP: done flag set");
        return;  // 上次搬运已完成, 直接跳APP
    }

    if (!IapIsUpdateRequested())
    {
        return;  // 无升级请求, 直接跳APP
    }

    LOG_INFO("IAP: update requested");
    BspExFlashRead(IAP_EXFLASH_HEAD_ADDR, (uint8_t *)&stcHead, sizeof(stcHead));

    if (IapVerifyImage(&stcHead, &u32ImageSize))
    {
        LOG_INFO("IAP: verify ok, size=%u", u32ImageSize);
        (void)IapProgramAppArea(u32ImageSize);  // 外部Flash镜像 → 内部Flash APP区
        IapClearUpdateFlags();                  // 清两标志扇区
        IapMarkUpdateDone();                    // 置完成标志
        NVIC_SystemReset();                     // 复位后走完成分支, 跳转新固件
    }
    else
    {
        LOG_WARN("IAP: verify NG, keep old app");
    }
}