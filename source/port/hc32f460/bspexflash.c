/**
 * @file    bspexflash.c
 * @brief   外部SPI Flash(W25Qxx)驱动实现(BSP层, GPIO模拟SPI)
 * @note    移植自华大bootloader工程app_driver_extern_flash:
 *          - 位操作时延NOP次数保持原工程数值(按原工程时钟整定, 换板需复核);
 *          - 忙等采用"WIP与WEL同时清零判空闲"的极性(原工程WEL版本语义)。
 */
#include <stdbool.h>
#include <stdint.h>
#include "bspexflash.h"
#include "bsppinmap.h"
#include "bspdelay.h"
#include "hc32_ll.h"

// ---------------- 引脚操作宏 ----------------
#define EXFLASH_CS_LOW()     GPIO_ResetPins(BSP_EXFLASH_CS_PORT, BSP_EXFLASH_CS_PIN)
#define EXFLASH_CS_HIGH()    GPIO_SetPins(BSP_EXFLASH_CS_PORT, BSP_EXFLASH_CS_PIN)
#define EXFLASH_SCK_LOW()    GPIO_ResetPins(BSP_EXFLASH_SCK_PORT, BSP_EXFLASH_SCK_PIN)
#define EXFLASH_SCK_HIGH()   GPIO_SetPins(BSP_EXFLASH_SCK_PORT, BSP_EXFLASH_SCK_PIN)
#define EXFLASH_MOSI_HIGH()  GPIO_SetPins(BSP_EXFLASH_MOSI_PORT, BSP_EXFLASH_MOSI_PIN)
#define EXFLASH_MOSI_LOW()   GPIO_ResetPins(BSP_EXFLASH_MOSI_PORT, BSP_EXFLASH_MOSI_PIN)
#define EXFLASH_MISO_READ()  GPIO_ReadInputPins(BSP_EXFLASH_MISO_PORT, BSP_EXFLASH_MISO_PIN)

// ---------------- 位操作时延(原工程整定值) ----------------

/** @brief 短延时(约1个位周期的建立段) */
static void SpiDelayShort(void)
{
    __NOP();
    __NOP();
    __NOP();
}

/** @brief 中延时(位周期主体) */
static void SpiDelayMid(void)
{
    uint8_t u8i;

    for (u8i = 0u; u8i < 10u; u8i++)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

/** @brief 长延时(采样后保持段) */
static void SpiDelayLong(void)
{
    uint8_t u8i;

    for (u8i = 0u; u8i < 30u; u8i++)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

/** @brief 拉低CS选中Flash */
static void SpiCsActive(void)
{
    EXFLASH_CS_LOW();
}

/** @brief 释放总线并拉高CS(时序与原工程一致) */
static void SpiCsIdle(void)
{
    EXFLASH_SCK_LOW();
    SpiDelayShort();
    EXFLASH_CS_HIGH();
    SpiDelayMid();
}

/** @brief 发送单字节(MSB先出, 时序与原工程一致) */
static void SpiSendByte(uint8_t u8Data)
{
    uint8_t u8i;

    for (u8i = 0u; u8i < 8u; u8i++)
    {
        if (u8Data & 0x80u)
        {
            EXFLASH_MOSI_HIGH();
        }
        else
        {
            EXFLASH_MOSI_LOW();
        }
        u8Data <<= 1;
        SpiDelayShort();
        EXFLASH_SCK_LOW();
        __NOP();
        SpiDelayMid();
        EXFLASH_SCK_HIGH();
        SpiDelayLong();
    }
}

/** @brief 接收单字节(MSB先入, 时序与原工程一致) */
static uint8_t SpiRecvByte(void)
{
    uint8_t u8i;
    uint8_t u8Data = 0u;

    for (u8i = 0u; u8i < 8u; u8i++)
    {
        u8Data <<= 1;
        EXFLASH_SCK_LOW();
        SpiDelayMid();
        if (EXFLASH_MISO_READ())
        {
            u8Data |= 1u;
        }
        EXFLASH_SCK_HIGH();
        SpiDelayMid();
    }
    EXFLASH_SCK_LOW();

    return u8Data;
}

/** @brief 发送单命令码(独立CS包络, 如写使能/唤醒) */
static void SpiSendCommand(uint8_t u8Cmd)
{
    SpiCsActive();
    SpiSendByte(u8Cmd);
    SpiCsIdle();
}

// ---------------- 对外接口(见bspexflash.h) ----------------

/** @brief 外部Flash初始化: CS/SCK/MOSI输出(默认高), MISO输入 */
void BspExFlashInit(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir   = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;  // 空闲态CS/SCK/MOSI均为高
    (void)GPIO_Init(BSP_EXFLASH_CS_PORT, BSP_EXFLASH_CS_PIN, &stcGpioInit);
    (void)GPIO_Init(BSP_EXFLASH_SCK_PORT, BSP_EXFLASH_SCK_PIN, &stcGpioInit);
    (void)GPIO_Init(BSP_EXFLASH_MOSI_PORT, BSP_EXFLASH_MOSI_PIN, &stcGpioInit);

    stcGpioInit.u16PinDir   = PIN_DIR_IN;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    (void)GPIO_Init(BSP_EXFLASH_MISO_PORT, BSP_EXFLASH_MISO_PIN, &stcGpioInit);
}

/**
 * @brief  唤醒外部Flash: 释放深度掉电 + 等待起振
 * @note   1.从deep power down中恢复
 *         CS#引脚低电平，切换至AB指令并将CS#引脚拉高；设备需要等待tRES1时间周期后才能恢复正常运行
 *         tRES1: 20us
 *         2.非deep power down下获取设备id时：
 *         CS#引脚置低电平，发送AB指令后接三个空字节;随后在SCLK信号下降沿将设备ID逐位读出，最后CS#引脚置高电平
 *         3. 从deep power down中释放并获取设备id时，
 *         CS#引脚置为高电平时，保持tRES2时间。在ERASE,PROGREAN,Write cycle(WIP=1)期间，该指令被忽略
 *         tRES2: 20us
 */
void BspExFlashWakeUp(void)
{
    uint16_t u16i;

    SpiSendCommand(EXFLASH_CMD_RELEASE_DPD);
    BspDelayUs(25);
}

/** @brief 从外部Flash连续读数据(单次CS包络, 0x03命令+24位地址+数据) */
void BspExFlashRead(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len)
{
    SpiCsActive();
    SpiSendByte(EXFLASH_CMD_READ_DATA);
    SpiSendByte((uint8_t)(u32Addr >> 16));  // 地址高字节在前
    SpiSendByte((uint8_t)(u32Addr >> 8));
    SpiSendByte((uint8_t)u32Addr);
    while (u32Len != 0u)
    {
        *pu8Buf++ = SpiRecvByte();
        u32Len--;
    }
    SpiCsIdle();
}

/**
 * @brief  向外部Flash写数据: 写使能→页编程→等空闲, 循环至写完
 * @note   页内地址不对齐时自动在256B页边界断开分次编程
 */
void BspExFlashWrite(uint32_t u32Addr, const uint8_t *pu8Buf, uint32_t u32Len)
{
    while (u32Len != 0u)
    {
        SpiSendCommand(EXFLASH_CMD_WRITE_ENABLE);

        SpiCsActive();
        SpiSendByte(EXFLASH_CMD_PAGE_PROGRAM);
        SpiSendByte((uint8_t)(u32Addr >> 16));
        SpiSendByte((uint8_t)(u32Addr >> 8));
        SpiSendByte((uint8_t)u32Addr);
        while (u32Len != 0u)
        {
            SpiSendByte(*pu8Buf++);
            u32Len--;
            u32Addr++;
            if (0u == (uint8_t)u32Addr)  // 低8位回卷说明到达页边界
            {
                break;
            }
        }
        SpiCsIdle();

        (void)BspExFlashWaitIdle(1000u);  // 等待本页编程完成
    }
}

/** @brief 擦除外部Flash一个扇区: 写使能→扇区擦除→等空闲 */
void BspExFlashSectorErase(uint32_t u32Addr)
{
    SpiSendCommand(EXFLASH_CMD_WRITE_ENABLE);

    SpiCsActive();
    SpiSendByte(EXFLASH_CMD_SECTOR_ERASE);
    SpiSendByte((uint8_t)(u32Addr >> 16));
    SpiSendByte((uint8_t)(u32Addr >> 8));
    SpiSendByte((uint8_t)u32Addr);
    SpiCsIdle();

    (void)BspExFlashWaitIdle(10000u);  // 扇区擦除典型45ms, 上限放宽
}

/**
 * @brief  等待外部Flash空闲: 轮询状态寄存器, WIP与WEL同时为0判空闲
 * @note   0x05命令连续读会重复返回状态字节, 故两个状态字节的bit0/bit1都检查
 *         (极性沿用原工程WEL版本; 原WIP-only版本判忙条件写反, 实际总是空转到超时)
 */
bool BspExFlashWaitIdle(uint16_t u16TimeoutMs)
{
    uint8_t  au8Status[2];
    uint16_t u16Inner;

    do
    {
        u16Inner = 5u;  // 每轮内层轮询次数, 约等效1ms(按SPI速度整定)
        do
        {
            SpiCsActive();
            SpiSendByte(EXFLASH_CMD_READ_STATUS);
            au8Status[0] = SpiRecvByte();
            au8Status[1] = SpiRecvByte();
            SpiCsIdle();

            if (((au8Status[0] & 0x03u) == 0u) && ((au8Status[1] & 0x03u) == 0u))
            {
                return true;  // WIP/WEL均清零, 空闲
            }
        } while (--u16Inner);
    } while (--u16TimeoutMs);

    return false;  // 超时仍忙
}