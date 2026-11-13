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
#include "apptimer.h"
#include "jumpapp.h"

/**
 * @brief  系统初始化: 依次拉起各层(顺序有时钟依赖, 勿随意调整)
 */
static void SystemInitAll(void)
{
    LL_PERIPH_WE(LL_PERIPH_ALL);  // 解锁外写保护

    RttLogInit();                 // log模块+RTT后端
    BspClkInit();                 // 时钟(200MHz)
    BspLedInit();                 // LED
    AppTimerInit();               // 时间片调度(内部初始化SysTick)

    LL_PERIPH_WP(LL_PERIPH_ALL);  // 重新上写保护
}

/**
 * @brief  应用入口: 初始化最小系统后跳转APP
 */
int main(void)
{
    SystemInitAll();
    LOG_ERROR("SystemCoreClock: %uHz", SystemCoreClock);
    LOG_ERROR("Boot V0.1");
    LOG_ERROR("->BOOT");

    // todo 防空验证: APP区栈顶为0xFFFFFFFF说明无有效固件, 停在此处防生产事故
    // if (0xFFFFFFFFu == *(volatile uint32_t *)BOOT_APP_START_ADDR) { while (1) {} }

    // 读外部Flash升级流程在此扩展(校验CRC后搬运到APP区)

    // __disable_irq();  // 跳转前按需关闭全局中断(APP入口自行重新使能)
    JumpToApp(BOOT_APP_START_ADDR);

    while (1)
    {
        // 正常流程不会回到这里
    }
}
