/**
 * @file    bspdelay.c
 * @brief   毫秒/微秒延时实现(BSP层)
 * @note    移植自My-OTA工程bsp_delay.c; 改用LL库SysTick_GetTick()
 *          消除了原版对app层AppTimerGetTick的逆向依赖(port不得依赖app)
 */
#include "bspdelay.h"
#include "hc32_ll_utility.h"

/** @brief 毫秒延时: 等待SysTick节拍走过指定毫秒数 */
void BspDelayMs(uint32_t u32Ms)
{
    uint32_t u32Start = SysTick_GetTick();
    uint32_t u32Cycles = 0;

    while ((SysTick_GetTick() - u32Start) < u32Ms)
    {
        // 占位循环: 将来此处定期喂狗, 避免长延时触发看门狗
        if (u32Cycles++ >= 100u)
        {
            u32Cycles = 0;
        }
    }
}

/** @brief 微秒延时: 忙等NOP
 *  200MHz主频下实测每循环约6个时钟周期, 200/6≈33循环/us */
void BspDelayUs(uint32_t u32Us)
{
    uint32_t u32Cycles = u32Us * 33u;

    while (u32Cycles--)
    {
        __NOP();
    }
}
