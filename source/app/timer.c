/**
 * @file    timer.c
 * @brief   前后台版时间片调度+SysTick中断实现(归app层所有)
 * @note    SysTick是调度资源——本bootloader在此拥有;
 *          跳转APP后由APP固件自行重新初始化
 */
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "hc32_ll_utility.h"

typedef void (*TimerTaskFn)(void);

// 1s任务表: 为了提高实时性, 1s任务拆分成100个10ms时间片分时执行
// (槽位N的任务在每秒的第N×10ms执行; 槽位即执行时刻, 按需错峰登记)
static const TimerTaskFn g_apfnTimer1sTaskList[] = {
    // bootloader暂无周期任务, 升级流程扩展时在此登记
};

static uint32_t      g_u32SystemTickMs  = 0;  // 系统毫秒节拍
static volatile bool g_b10msFlag        = false;
static uint16_t      g_u16Tick10msCnt   = 0;
static uint8_t       g_u8TimeSliceId    = 0;  // 1s任务的时间片编号(0~99)

/**
 * @brief  分时1s任务: 每10ms执行任务表中的一个槽位
 */
static void Timer1sTask(void)
{
    if (++g_u8TimeSliceId >= 100u)
    {
        g_u8TimeSliceId = 0;
    }

    if (g_u8TimeSliceId < (sizeof(g_apfnTimer1sTaskList) / sizeof(g_apfnTimer1sTaskList[0])))
    {
        g_apfnTimer1sTaskList[g_u8TimeSliceId]();
    }
}

/**
 * @brief  时间片调度初始化: 初始化SysTick(1ms)
 */
void TimerInit(void)
{
    if (LL_OK != SysTick_Init(1000U))
    {
        LOG_ERROR("SysTick init FAILED");
    }
}

/**
 * @brief  SysTick中断服务函数(1ms中断上下文)
 * @note   仅置标志/计数, 不做耗时操作
 */
void SysTick_Handler(void)
{
    SysTick_IncTick();

    g_u16Tick10msCnt++;
    if (g_u16Tick10msCnt >= 10u)
    {
        g_b10msFlag      = true;
        g_u16Tick10msCnt = 0;
    }
    g_u32SystemTickMs += 1u;
}

/** @brief 时间片任务调度: 10ms标志到了执行一个槽位 */
void TimerTaskRun(void)
{
    if (g_b10msFlag)
    {
        g_b10msFlag = false;
        Timer1sTask();
        // 实时性要求较高的任务在此追加
    }
}

/** @brief 获取系统毫秒节拍 */
uint32_t TimerGetTick(void)
{
    return g_u32SystemTickMs;
}