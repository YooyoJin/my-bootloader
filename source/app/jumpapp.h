/**
 * @file    jumpapp.h
 * @brief   跳转APP核心业务(bootloader存在的意义)
 */
#ifndef JUMPAPP_H
#define JUMPAPP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// APP起始地址: 该地址存放初始SP, +4存放复位向量
// 0x4000 = 16KB, 即bootloader允许占用的最大FLASH空间
#define BOOT_APP_START_ADDR   (0x4000u)

/**
 * @brief  跳转到APP
 * @param  u32AppAddr APP起始地址(该地址存放初始SP, +4存放复位向量)
 * @note   使用前应: 关全局中断+复位外设+关用到的时钟
 */
void JumpToApp(uint32_t u32AppAddr);

#ifdef __cplusplus
}
#endif

#endif // JUMPAPP_H
