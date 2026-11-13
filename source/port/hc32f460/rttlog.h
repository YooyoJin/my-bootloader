/**
 * @file    rttlog.h
 * @brief   日志模块的RTT输出后端(HC32F460, port层)
 */
#ifndef RTTLOG_H
#define RTTLOG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  初始化SEGGER RTT并注入log模块
 * @note   内部完成: SEGGER_RTT_Init() + LogInit(RttLogOutput)
 *         之后LOG_XXX宏的输出经RTT通道0发出
 */
void RttLogInit(void);

#ifdef __cplusplus
}
#endif

#endif // RTTLOG_H
