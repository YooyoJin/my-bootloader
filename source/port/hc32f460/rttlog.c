/**
 * @file    rttlog.c
 * @brief   日志模块的RTT输出后端实现
 * @note    RTT控制块地址打印保留(上位机RTT Viewer搜索RAM用):
 *          0x1FFF8000或0x10000000区域, 直接全范围192M RAM搜索
 */
#include "rttlog.h"
#include "log.h"
#include "SEGGER_RTT.h"

/** @brief log模块输出钩子: 把一行日志写入RTT通道0 */
static void RttLogOutput(const char *pcStr)
{
    SEGGER_RTT_WriteString(0, pcStr);
}

/** @brief 初始化RTT并注入log模块 */
void RttLogInit(void)
{
    SEGGER_RTT_Init();
    LogInit(RttLogOutput);
    LOG_INFO("_SEGGER_RTT: 0x%08X", (uint32_t)&_SEGGER_RTT);
}
