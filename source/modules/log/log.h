/**
 * @file    log.h
 * @brief   分级日志模块(纯逻辑,输出后端钩子化)
 *
 * 模块本身不依赖任何硬件——输出到哪(RTT/UART/仿真终端)由
 * 移植层实现LogOutputFn钩子后注入(HC32F460移植层见port/rttlog.c)。
 *
 * 配置:
 *   - LOG_ENABLE    发布版置0, 日志代码编译期完全移除
 *   - LOG_SHOW_LEVEL运行时过滤的最高级别
 *
 * @version v0.1.0 (同步自my-libs仓库modules/log, 后端改为钩子注入)
 */
#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 日志级别(数值越大越宽松, 运行时按">当前级别则丢弃"过滤)
typedef enum
{
    LogLevelNone  = 0,
    LogLevelError = 1,
    LogLevelWarn  = 2,
    LogLevelDebug = 3,
    LogLevelInfo  = 4,
} LogLevel;

// ---------------- 配置区 ----------------
#define LOG_ENABLE     1              // 发布版本置0, 全部日志编译期移除
#define LOG_SHOW_LEVEL LogLevelError  // 运行时显示的最高级别(bootloader保持ERROR)
// ----------------------------------------

#if LOG_ENABLE
#define LOG_ERROR(...) LogPrint(LogLevelError, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  LogPrint(LogLevelWarn,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) LogPrint(LogLevelDebug, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  LogPrint(LogLevelInfo,  __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#endif

/** @brief 输出后端钩子: 接收一行完整日志字符串(以'\n'结尾) */
typedef void (*LogOutputFn)(const char *pcStr);

/**
 * @brief  日志模块初始化,注入输出后端
 * @param  pfnOutput 输出函数(RTT写串/UART发送/printf等), 传NULL则丢弃所有日志
 */
void LogInit(LogOutputFn pfnOutput);

/** @brief 运行时修改日志过滤级别 */
void LogSetLevel(LogLevel eLevel);

/**
 * @brief  日志输出入口(一般不直接调用,用LOG_XXX宏)
 * @note   自动附加"[级别] 文件名:行号"前缀与'\n'结尾, 缓冲区128字节
 */
void LogPrint(LogLevel eLevel, const char *pcFile, uint32_t u32Line, const char *pcFormat, ...);

#ifdef __cplusplus
}
#endif

#endif // LOG_H
