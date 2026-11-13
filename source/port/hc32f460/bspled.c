/**
 * @file    bspled.c
 * @brief   状态LED控制实现(BSP层)
 * @note    移植自原bsp_led.c, 逻辑未改动(闪烁周期保持100ms)
 */
#include "bspled.h"
#include "bspdelay.h"
#include "bspboardtype.h"
#include "hc32_ll_gpio.h"

static BspGpioPin_T g_stStatusLed;

/** @brief LED初始化: 输出模式+默认熄灭 */
void BspLedInit(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir  = PIN_DIR_OUT;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    g_stStatusLed.u16Port  = BSP_LED0_PORT;
    g_stStatusLed.u16Pin   = BSP_LED0_PIN;
    (void)GPIO_Init(g_stStatusLed.u16Port, g_stStatusLed.u16Pin, &stcGpioInit);

    BspLedOff();
}

/** @brief 点亮(低电平驱动) */
void BspLedOn(void)
{
    GPIO_ResetPins(g_stStatusLed.u16Port, g_stStatusLed.u16Pin);
}

/** @brief 熄灭 */
void BspLedOff(void)
{
    GPIO_SetPins(g_stStatusLed.u16Port, g_stStatusLed.u16Pin);
}

/** @brief 翻转 */
void BspLedToggle(void)
{
    GPIO_TogglePins(g_stStatusLed.u16Port, g_stStatusLed.u16Pin);
}

/** @brief 阻塞闪烁: 每次亮灭各100ms */
void BspLedBlinking(uint8_t u8Times)
{
    BspLedOff();
    while (u8Times--)
    {
        BspLedToggle();
        BspDelayMs(100);
        BspLedToggle();
        BspDelayMs(100);
    }
}
