/**
 *******************************************************************************
 * @file  bsp_board_type.h
 * @brief This file contains the BSP layer board pin definition types.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2025-11-07
 @endverbatim
 *******************************************************************************
 */
#ifndef __BSP_BOARD_TYPE_H__
#define __BSP_BOARD_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hc32_ll.h"

#define EMPTY                             0

/*typedef struct {
    uint8_t hw_major;    // 硬件主版本
    uint8_t hw_minor;    // 硬件次版本
    uint8_t bsp_major;   // BSP主版本
    uint8_t bsp_minor;   // BSP次版本
    const char *chip_id; // 芯片型号
    uint32_t features;   // 功能位图
} bsp_info_t;*/

// GPIO Pin definition
typedef struct {
    uint16_t u16Port;
    uint16_t u16Pin;
} BspGpioPin_T;


// BSP只对硬件负责，在应用层赋予意义

// XTAL
#define BSP_XTAL_IN_PORT                  GPIO_PORT_H
#define BSP_XTAL_IN_PIN                   GPIO_PIN_00
#define BSP_XTAL_OUT_PORT                 GPIO_PORT_H
#define BSP_XTAL_OUT_PIN                  GPIO_PIN_01

// LED
#define BSP_LED0_PORT                     GPIO_PORT_C
#define BSP_LED0_PIN                      GPIO_PIN_12
#define BSP_LED1_PORT                     EMPTY
#define BSP_LED1_PIN                      EMPTY
#define BSP_LED2_PORT                     EMPTY
#define BSP_LED2_PIN                      EMPTY
#define BSP_LED3_PORT                     EMPTY
#define BSP_LED3_PIN                      EMPTY
#define BSP_LED4_PORT                     EMPTY
#define BSP_LED4_PIN                      EMPTY

// 485
// UART1-485PC(A1B1)
#define BSP_4850_RD_PORT                  GPIO_PORT_C
#define BSP_4850_RD_PIN                   GPIO_PIN_05
#define BSP_4850_TX_PORT                  GPIO_PORT_B
#define BSP_4850_TX_PIN                   GPIO_PIN_00
#define BSP_4850_TX_ALTERNATE_FUNC        GPIO_FUNC_32
#define BSP_4850_RX_PORT                  GPIO_PORT_B
#define BSP_4850_RX_PIN                   GPIO_PIN_01
#define BSP_4850_RX_ALTERNATE_FUNC        GPIO_FUNC_33

// UART2-485LJ(A4B4)
#define BSP_4851_RD_PORT                  GPIO_PORT_D
#define BSP_4851_RD_PIN                   GPIO_PIN_07
#define BSP_4851_TX_PORT                  GPIO_PORT_C
#define BSP_4851_TX_PIN                   GPIO_PIN_11
#define BSP_4851_TX_ALTERNATE_FUNC        GPIO_FUNC_36
#define BSP_4851_RX_PORT                  GPIO_PORT_C
#define BSP_4851_RX_PIN                   GPIO_PIN_10
#define BSP_4851_RX_ALTERNATE_FUNC        GPIO_FUNC_37

// UART3-485QD
#define BSP_4852_RD_PORT                  GPIO_PORT_E
#define BSP_4852_RD_PIN                   GPIO_PIN_01
#define BSP_4852_TX_PORT                  GPIO_PORT_E
#define BSP_4852_TX_PIN                   GPIO_PIN_02
#define BSP_4852_TX_ALTERNATE_FUNC        GPIO_FUNC_32
#define BSP_4852_RX_PORT                  GPIO_PORT_E
#define BSP_4852_RX_PIN                   GPIO_PIN_03
#define BSP_4852_RX_ALTERNATE_FUNC        GPIO_FUNC_33

// UART2-485XK
#define BSP_4853_RD_PORT                  GPIO_PORT_B
#define BSP_4853_RD_PIN                   GPIO_PIN_06
#define BSP_4853_TX_PORT                  GPIO_PORT_B
#define BSP_4853_TX_PIN                   GPIO_PIN_07
#define BSP_4853_TX_ALTERNATE_FUNC        GPIO_FUNC_36
#define BSP_4853_RX_PORT                  GPIO_PORT_B
#define BSP_4853_RX_PIN                   GPIO_PIN_05
#define BSP_4853_RX_ALTERNATE_FUNC        GPIO_FUNC_37

// KEY
#define BSP_KEY0_PORT                     EMPTY
#define BSP_KEY0_PIN                      EMPTY
#define BSP_KEY1_PORT                     EMPTY
#define BSP_KEY1_PIN                      EMPTY
#define BSP_KEY2_PORT                     EMPTY
#define BSP_KEY2_PIN                      EMPTY
#define BSP_KEY3_PORT                     EMPTY
#define BSP_KEY3_PIN                      EMPTY

// TM1637 Port/Pin definition
#define BSP_TM1637_CLK_PORT               EMPTY
#define BSP_TM1637_CLK_PIN                EMPTY
#define BSP_TM1637_DATA_PORT              EMPTY
#define BSP_TM1637_DATA_PIN               EMPTY

// E2PROM I2C Port/Pin definition
#define BSP_M24C32_EEPROM_I2C_SDA_PORT    GPIO_PORT_D
#define BSP_M24C32_EEPROM_I2C_SDA_PIN     GPIO_PIN_01
#define BSP_M24C32_EEPROM_I2C_SCL_PORT    GPIO_PORT_D
#define BSP_M24C32_EEPROM_I2C_SCL_PIN     GPIO_PIN_02
#define BSP_M24C32_EEPROM_WP_PORT         GPIO_PORT_D
#define BSP_M24C32_EEPROM_WP_PIN          GPIO_PIN_03

// RELAY Port/Pin definition
#define BSP_RY1_PORT                      GPIO_PORT_A
#define BSP_RY1_PIN                       GPIO_PIN_09
#define BSP_RY2_PORT                      GPIO_PORT_A
#define BSP_RY2_PIN                       GPIO_PIN_08
#define BSP_RY3_PORT                      GPIO_PORT_C
#define BSP_RY3_PIN                       GPIO_PIN_09
#define BSP_RY4_PORT                      GPIO_PORT_C
#define BSP_RY4_PIN                       GPIO_PIN_08
#define BSP_RY5_PORT                      GPIO_PORT_C
#define BSP_RY5_PIN                       GPIO_PIN_07
#define BSP_RY6_PORT                      GPIO_PORT_D
#define BSP_RY6_PIN                       GPIO_PIN_08
#define BSP_RY7_PORT                      GPIO_PORT_B
#define BSP_RY7_PIN                       GPIO_PIN_15
#define BSP_RY8_PORT                      GPIO_PORT_B
#define BSP_RY8_PIN                       GPIO_PIN_14
#define BSP_RY9_PORT                      GPIO_PORT_A  // Big RY
#define BSP_RY9_PIN                       GPIO_PIN_10
#define BSP_RY10_PORT                     GPIO_PORT_A
#define BSP_RY10_PIN                      GPIO_PIN_12
#define BSP_RY11_PORT                     GPIO_PORT_D
#define BSP_RY11_PIN                      GPIO_PIN_05
#define BSP_RY12_PORT                     GPIO_PORT_D
#define BSP_RY12_PIN                      GPIO_PIN_06

// ADC Sample Port/Pin definition
#define BSP_ADC_TA_PORT                   GPIO_PORT_A  // 温度传感器
#define BSP_ADC_TA_PIN                    GPIO_PIN_07
#define BSP_ADC_TE_PORT                   GPIO_PORT_A
#define BSP_ADC_TE_PIN                    GPIO_PIN_06
#define BSP_ADC_TO_PORT                   GPIO_PORT_A
#define BSP_ADC_TO_PIN                    GPIO_PIN_05
#define BSP_ADC_T1_PORT                   GPIO_PORT_C
#define BSP_ADC_T1_PIN                    GPIO_PIN_02
#define BSP_ADC_T2_PORT                   GPIO_PORT_C
#define BSP_ADC_T2_PIN                    GPIO_PIN_03
#define BSP_ADC_T3_PORT                   GPIO_PORT_A
#define BSP_ADC_T3_PIN                    GPIO_PIN_00
#define BSP_ADC_T4_PORT                   GPIO_PORT_A
#define BSP_ADC_T4_PIN                    GPIO_PIN_01
#define BSP_ADC_T5_PORT                   GPIO_PORT_A
#define BSP_ADC_T5_PIN                    GPIO_PIN_02
#define BSP_ADC_T6_PORT                   GPIO_PORT_A
#define BSP_ADC_T6_PIN                    GPIO_PIN_03
#define BSP_ADC_T7_PORT                   GPIO_PORT_A
#define BSP_ADC_T7_PIN                    GPIO_PIN_04
#define BSP_ADC_T8_PORT                   GPIO_PORT_A
#define BSP_ADC_T8_PIN                    GPIO_PIN_07
#define BSP_ADC_PT1_PORT                  GPIO_PORT_C  // 压力传感器
#define BSP_ADC_PT1_PIN                   GPIO_PIN_00
#define BSP_ADC_PT2_PORT                  GPIO_PORT_C
#define BSP_ADC_PT2_PIN                   GPIO_PIN_01

// SW Port/Pin definition
#define BSP_SW1_PORT                      GPIO_PORT_E
#define BSP_SW1_PIN                       GPIO_PIN_08
#define BSP_SW2_PORT                      GPIO_PORT_E
#define BSP_SW2_PIN                       GPIO_PIN_09
#define BSP_SW3_PORT                      GPIO_PORT_E
#define BSP_SW3_PIN                       GPIO_PIN_10
#define BSP_SW4_PORT                      GPIO_PORT_E
#define BSP_SW4_PIN                       GPIO_PIN_11
#define BSP_SW5_PORT                      GPIO_PORT_E
#define BSP_SW5_PIN                       GPIO_PIN_12
#define BSP_SW6_PORT                      GPIO_PORT_E
#define BSP_SW6_PIN                       GPIO_PIN_13
#define BSP_SW7_PORT                      GPIO_PORT_E
#define BSP_SW7_PIN                       GPIO_PIN_14

// SelfTest/TimeCompress Port/Pin definition
#define BSP_SS_PORT                       GPIO_PORT_D
#define BSP_SS_PIN                        GPIO_PIN_00


#ifdef __cplusplus
}
#endif

#endif /* __BSP_BOARD_TYPE_H__ */
