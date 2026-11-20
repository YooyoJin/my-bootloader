/**
 * @file    iapupdate.h
 * @brief   IAP升级核心业务: 外部Flash暂存镜像 → CRC校验 → 搬运到内部Flash APP区
 *
 * 外部SPI Flash升级协议布局(与App侧收包逻辑约定, 布局沿用华大工程不变):
 *   | 地址     | 内容                                                       |
 *   |---------|------------------------------------------------------------|
 *   | 0x000000| 16B升级头(镜像长度3B+起始地址3B+签名码3B+预留7B, 大端)   |
 *   | 0x000010| 固件镜像(长度=升级头镜像长度字段)                           |
 *   | 镜像尾部| 2B CRC16(Modbus)                                           |
 *   | 0x03EFF0| 升级完成标志扇区(2B: 0x36 0x63)                            |
 *   | 0x03FFF0| 升级请求标志扇区(2B: 0xA5 0x5A)                            |
 * CRC覆盖"升级头前9B + 预留7B(地址9~15) + 镜像 + 尾部CRC", 全流CRC余数为0判过。
 *
 * @version v0.1.0 (移植自华大bootloader工程bootloader_iap, 布局/校验算法/流程不变)
 */
#ifndef IAPUPDATE_H
#define IAPUPDATE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- 外部Flash升级协议布局 ----------------
#define IAP_EXFLASH_HEAD_ADDR           0x000000u                 // 升级头地址
#define IAP_EXFLASH_HEAD_SIZE           16u                       // 升级头大小
#define IAP_EXFLASH_IMAGE_START_ADDR    (IAP_EXFLASH_HEAD_ADDR + IAP_EXFLASH_HEAD_SIZE)  // 镜像起始地址
#define IAP_EXFLASH_DONE_FLAG_ADDR      0x03EFF0u                 // 升级完成标志地址(独立扇区)
#define IAP_EXFLASH_REQ_FLAG_ADDR       0x03FFF0u                 // 升级请求标志地址(独立扇区)

#define IAP_FLAG_DONE_BYTE0             0x36u                     // 完成标志魔数字节0
#define IAP_FLAG_DONE_BYTE1             0x63u                     // 完成标志魔数字节1
#define IAP_FLAG_REQ_BYTE0              0xA5u                     // 请求标志魔数字节0
#define IAP_FLAG_REQ_BYTE1              0x5Au                     // 请求标志魔数字节1

// ---------------- 镜像约束参数 ----------------
#define IAP_IMAGE_MAX_SIZE              500000u                   // 镜像最大字节数
#define IAP_IMAGE_ID_CODE               0x00000002u               // 镜像签名码(升级头签名字段须相等)
// 注: 原工程This_ID_Code意图固化为内部Flash 0x2000处的常量(GCC链接未实际生效, 仅作常量比对), 此处以宏等价实现
#define IAP_INNER_FLASH_SECTOR_SIZE     8192u                     // 内部Flash扇区大小(Bytes)
#define IAP_CRC_CHUNK_SIZE              256u                      // CRC流式校验单次读块大小

#pragma pack(1)
// 升级头(外部Flash前16B, 多字节字段大端; FOTA方案冻结后此结构禁止再变动)
typedef struct
{
    uint8_t au8ImageSize[3];  // 镜像字节数(大端)
    uint8_t au8StartAddr[3];  // 起始地址(大端, 仅参与CRC, 不参与判定)
    uint8_t au8Signature[3];  // 签名码(大端, 须等于IAP_IMAGE_ID_CODE)
    uint8_t au8Reserved[7];   // 预留(参与CRC)
} stcIapImageHead_t;
#pragma pack()

/**
 * @brief  IAP升级入口: 检查外部Flash标志位决定是否搬运镜像
 * @note   流程: 完成标志已置→直接返回; 请求标志已置且CRC通过→搬运镜像到APP区→
 *         清两标志扇区→置完成标志→系统复位(复位后走完成分支跳新固件);
 *         其余情况直接返回, 由调用方继续跳转APP
 */
void IapUpdate(void);

#ifdef __cplusplus
}
#endif

#endif // IAPUPDATE_H