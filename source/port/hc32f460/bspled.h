/**
 * @file    bspled.h
 * @brief   状态LED控制(BSP层)
 */
#ifndef BSPLED_H
#define BSPLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @brief LED初始化: 输出模式+默认熄灭 */
void BspLedInit(void);

/** @brief 点亮(低电平驱动) */
void BspLedOn(void);

/** @brief 熄灭 */
void BspLedOff(void);

/** @brief 翻转 */
void BspLedToggle(void);

/**
 * @brief  阻塞闪烁: 每次亮灭各100ms
 * @param  u8Times 闪烁次数
 */
void BspLedBlinking(uint8_t u8Times);

#ifdef __cplusplus
}
#endif

#endif // BSPLED_H
