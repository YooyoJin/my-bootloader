/**
 * @file    main.c
 * @brief   Bootloader入口——组装根
 *
 * 本文件是耦合合法区: 认识各层并完成组装接线; 流程为
 * 初始化最小系统(RTT日志/时钟/LED)后直接跳转APP。
 */
#include "main.h"
#include "hc32_ll.h"
#include "log.h"
#include "rttlog.h"
#include "bspclk.h"
#include "bspled.h"
#include "bspdelay.h"
#include "bspexflash.h"
#include "timer.h"
#include "jumpapp.h"
#include "iapupdate.h"

/**
 * @brief  系统初始化: 依次拉起各层(顺序有时钟依赖, 勿随意调整)
 */
static void SystemInitAll(void)
{
    LL_PERIPH_WE(LL_PERIPH_ALL);  // 解锁外写保护

    RttLogInit();                 // log模块+RTT后端
    BspClkInit();                 // 时钟(200MHz)
    BspLedInit();                 // LED
    BspExFlashInit();             // 外部Flash(GPIO模拟SPI引脚)
    TimerInit();                  // 时间片调度(内部初始化SysTick)

    LL_PERIPH_WP(LL_PERIPH_ALL);  // 重新上写保护
}

/**
 * @brief  应用入口: 初始化最小系统 → IAP升级判定 → 跳转APP
 */
int main(void)
{
    SystemInitAll();
    LOG_INFO("SystemCoreClock: %uHz", SystemCoreClock);
    LOG_INFO("Boot v0.2.0");
    LOG_INFO("->Boot");
    // v0.1.0 Boot首版本
    // v0.2.0 移植华大工程IAP升级流程(外部Flash暂存→CRC校验→搬运→跳转)

    // todo 防空验证: APP区栈顶为0xFFFFFFFF说明无有效固件, 停在此处防生产事故
    // if (0xFFFFFFFFu == *(volatile uint32_t *)BOOT_APP_START_ADDR) { while (1) {} }

    BspDelayMs(200);  // 上电延时等待外部Flash供电稳定(对齐原工程200ms)
    BspExFlashWakeUp();  // 释放外部Flash深度掉电



    while (1)
    {
        // 正常流程不会回到这里
    }

    IapUpdate();  // 升级判定: 有请求则搬运镜像并复位, 无请求落空继续

    LOG_INFO("I'm ready to jump!");
    __disable_irq();  // 跳转前按需关闭全局中断(APP入口自行重新使能)
    JumpToApp(BOOT_APP_START_ADDR);

    while (1)
    {
        // 正常流程不会回到这里
    }
}
