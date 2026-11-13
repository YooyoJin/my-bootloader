/**
 * @file    bspdelay.h
 * @brief   毫秒/微秒延时(BSP层)
 */
#ifndef BSPDELAY_H
#define BSPDELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @brief 毫秒延时(基于SysTick节拍, 可被中断打断) */
void BspDelayMs(uint32_t u32Ms);

/** @brief 微秒延时(忙等NOP, 精度约±20%) */
void BspDelayUs(uint32_t u32Us);

#ifdef __cplusplus
}
#endif

#endif // BSPDELAY_H
