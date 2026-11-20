/**
 * @file    bspexflash.h
 * @brief   外部SPI Flash(W25Qxx)驱动接口(BSP层, GPIO模拟SPI)
 * @note    引脚定义见bsppinmap.h; 升级镜像的外部Flash布局见iapupdate.h
 *
 * @version v0.1.0 (移植自华大bootloader工程app_driver_extern_flash, 时序逻辑保持原值)
 */
#ifndef BSPEXFLASH_H
#define BSPEXFLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// W25Qxx命令码(仅本驱动用到的部分)
#define EXFLASH_CMD_WRITE_ENABLE    0x06u  // 写使能
#define EXFLASH_CMD_READ_STATUS     0x05u  // 读状态寄存器
#define EXFLASH_CMD_READ_DATA       0x03u  // 读数据
#define EXFLASH_CMD_PAGE_PROGRAM    0x02u  // 页编程
#define EXFLASH_CMD_SECTOR_ERASE    0x20u  // 扇区擦除(4KB)
#define EXFLASH_CMD_RELEASE_DPD     0xABu  // 深度掉电释放/唤醒

// Flash几何参数
#define EXFLASH_PAGE_SIZE           256u   // 页大小(Bytes)
#define EXFLASH_SECTOR_SIZE         4096u  // 扇区大小(Bytes)

/**
 * @brief  外部Flash初始化: GPIO模拟SPI引脚配置(CS/SCK/MOSI推挽输出, MISO输入)
 * @note   需在外设写保护解锁区间内调用
 */
void BspExFlashInit(void);

/** @brief 唤醒外部Flash(发送深度掉电释放命令+等待起振) */
void BspExFlashWakeUp(void);

/**
 * @brief  从外部Flash连续读数据(单次CS, 地址自增)
 * @param  u32Addr 起始地址
 * @param  pu8Buf  读出缓冲区
 * @param  u32Len  字节数
 */
void BspExFlashRead(uint32_t u32Addr, uint8_t *pu8Buf, uint32_t u32Len);

/**
 * @brief  向外部Flash写数据(按256B页自动分段编程, 段间等待空闲)
 * @param  u32Addr 起始地址
 * @param  pu8Buf  写入数据
 * @param  u32Len  字节数
 */
void BspExFlashWrite(uint32_t u32Addr, const uint8_t *pu8Buf, uint32_t u32Len);

/**
 * @brief  擦除外部Flash一个扇区(4KB, 内部等待擦除完成)
 * @param  u32Addr 扇区内任意地址(建议扇区对齐)
 */
void BspExFlashSectorErase(uint32_t u32Addr);

/**
 * @brief  等待外部Flash空闲(WIP/WEL状态位轮询)
 * @param  u16TimeoutMs 超时(约ms单位)
 * @retval true 空闲; false 超时仍忙
 */
bool BspExFlashWaitIdle(uint16_t u16TimeoutMs);

#ifdef __cplusplus
}
#endif

#endif // BSPEXFLASH_H