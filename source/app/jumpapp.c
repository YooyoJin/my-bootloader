/**
 * @file    jumpapp.c
 * @brief   跳转APP实现: 取APP栈顶/复位向量→设MSP→跳转
 */
#include "jumpapp.h"
#include "hc32_ll.h"

/**
 * @brief  Bootloader跳转到APP
 * @note   使用前应: 关全局中断+复位外设+关用到的时钟
 */
void JumpToApp(uint32_t u32AppAddr)
{
    uint32_t u32AppSp = *(volatile uint32_t *)u32AppAddr;        // 1. 拿APP的堆栈指针
    uint32_t u32AppPc = *(volatile uint32_t *)(u32AppAddr + 4u); // 2. 拿APP的复位向量
    void (*pfnAppReset)(void) = (void (*)(void))u32AppPc;

    __set_MSP(u32AppSp);   // 3. 设主堆栈
    pfnAppReset();         // 4. 跳!
}
