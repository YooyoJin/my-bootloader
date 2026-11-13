/**
 * @file    bspclk.h
 * @brief   HC32F460时钟初始化(BSP层)
 * @note    SysTick已移至app层——它是调度资源,归属取决于运行模型:
 *          前后台版在apptimer.c; FreeRTOS版由RTOS port自带
 */
#ifndef BSPCLK_H
#define BSPCLK_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 系统时钟初始化: XTAL→MPLL 200MHz主频+总线分频+SRAM/Flash等待周期 */
void BspClkInit(void);

#ifdef __cplusplus
}
#endif

#endif // BSPCLK_H
