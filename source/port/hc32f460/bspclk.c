/**
 * @file    bspclk.c
 * @brief   HC32F460时钟初始化+SysTick实现(BSP层)
 * @note    移植自My-OTA工程bsp_clk.c, 逻辑未改动
 */
#include "log.h"
#include "bspboardtype.h"
#include "bspclk.h"
#include "hc32_ll_clk.h"
#include "hc32_ll_gpio.h"
#include "hc32_ll_sram.h"
#include "hc32_ll_efm.h"
#include "hc32_ll_utility.h"
#include "hc32_ll_pwc.h"

/** @brief 系统时钟初始化: XTAL→MPLL 200MHz + 总线分频 + SRAM/Flash等待周期 */
void BspClkInit(void)
{
    stc_gpio_init_t stcGpioInit;

    // 总线时钟分频
    CLK_SetClockDiv(CLK_BUS_CLK_ALL, (CLK_HCLK_DIV1 | CLK_EXCLK_DIV2 | CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 |
                                      CLK_PCLK2_DIV4 | CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2));

    // SRAM读写等待周期
    SRAM_SetWaitCycle(SRAM_SRAM_ALL, SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);

    // Flash读等待周期
    EFM_SetWaitCycle(EFM_WAIT_CYCLE4);

    // 外部晶振
    {
        stc_clock_xtal_init_t stcXtalInit;
        (void)CLK_XtalStructInit(&stcXtalInit);
        stcXtalInit.u8State       = CLK_XTAL_ON;
        stcXtalInit.u8Drv         = CLK_XTAL_DRV_HIGH;
        stcXtalInit.u8Mode        = CLK_XTAL_MD_OSC;
        stcXtalInit.u8StableTime  = CLK_XTAL_STB_2MS;
        (void)CLK_XtalInit(&stcXtalInit);
    }

    // MPLL: XTAL 8MHz ×50 /2 → 200MHz
    {
        stc_clock_pll_init_t stcMPLLInit;
        (void)CLK_PLLStructInit(&stcMPLLInit);
        stcMPLLInit.PLLCFGR = 0UL;
        stcMPLLInit.PLLCFGR_f.PLLM = (1UL - 1UL);
        stcMPLLInit.PLLCFGR_f.PLLN = (50UL - 1UL);
        stcMPLLInit.PLLCFGR_f.PLLP = (2UL - 1UL);
        stcMPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
        stcMPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL);
        stcMPLLInit.u8PLLState = CLK_PLL_ON;
        stcMPLLInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
        (void)CLK_PLLInit(&stcMPLLInit);
    }

    // UPLL(USB等外设时钟源)
    {
        stc_clock_pllx_init_t stcUPLLInit;
        (void)CLK_PLLxStructInit(&stcUPLLInit);
        stcUPLLInit.PLLCFGR = 0UL;
        stcUPLLInit.PLLCFGR_f.PLLM = (2UL - 1UL); // 实际分频值=寄存器值+1
        stcUPLLInit.PLLCFGR_f.PLLN = (60UL - 1UL);
        stcUPLLInit.PLLCFGR_f.PLLP = (2UL - 1UL);
        stcUPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
        stcUPLLInit.PLLCFGR_f.PLLR = (5UL - 1UL);
        stcUPLLInit.u8PLLState = CLK_PLLX_ON;
        (void)CLK_PLLxInit(&stcUPLLInit);
    }

    // 126~200MHz区间GPIO读等待3周期
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);
    // 切换驱动能力
    PWC_HighSpeedToHighPerformance();
    // 系统时钟源切到PLL → SYSCLK 200MHz
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);

    // PH0 → XTAL-IN
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(BSP_XTAL_IN_PORT, BSP_XTAL_IN_PIN, &stcGpioInit);

    // PH1 → XTAL-OUT
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(BSP_XTAL_OUT_PORT, BSP_XTAL_OUT_PIN, &stcGpioInit);
}
