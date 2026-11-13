/**
 * @file    log.c
 * @brief   分级日志模块实现(格式化+级别过滤, 经钩子输出)
 * @note    同步自my-libs仓库modules/log, 逻辑未改动
 */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "log.h"

static LogLevel     g_eCurrentLevel = LOG_SHOW_LEVEL;
static LogOutputFn  g_pfnOutput     = NULL;

// 级别前缀字符串(下标与LogLevel对应)
static const char *g_apcLevelStr[] = {
    "",
    "[ERROR] ",
    "[WARN ] ",
    "[DEBUG] ",
    "[INFO ] "
};

/**
 * @brief  从文件全路径中截取文件名
 * @note   输入"E:/xx/source/main.c" → 返回"main.c"
 */
static const char *LogGetFileName(const char *pcFile)
{
    const char *pcPos = strrchr(pcFile, '/');
    if (pcPos)
    {
        return pcPos + 1;
    }
    pcPos = strrchr(pcFile, '\\');
    if (pcPos)
    {
        return pcPos + 1;
    }
    return pcFile;
}

/** @brief 日志模块初始化,注入输出后端 */
void LogInit(LogOutputFn pfnOutput)
{
    g_pfnOutput = pfnOutput;
}

/** @brief 运行时修改日志过滤级别 */
void LogSetLevel(LogLevel eLevel)
{
    g_eCurrentLevel = eLevel;
}

/** @brief 日志输出入口: 组前缀→格式化→补换行→经钩子输出 */
void LogPrint(LogLevel eLevel, const char *pcFile, uint32_t u32Line, const char *pcFormat, ...)
{
    char cBuffer[128];
    int  iLen;

    if ((eLevel > g_eCurrentLevel) || (NULL == g_pfnOutput))
    {
        return;
    }

    // 前缀: 级别 + 文件名 + 行号
    iLen = snprintf(cBuffer, sizeof(cBuffer), "%s%s:%u ",
                    g_apcLevelStr[eLevel], LogGetFileName(pcFile), (unsigned int)u32Line);
    if (iLen <= 0 || iLen >= (int)sizeof(cBuffer))
    {
        return;
    }

    // 用户消息
    {
        va_list args;
        va_start(args, pcFormat);
        vsnprintf(cBuffer + iLen, sizeof(cBuffer) - iLen, pcFormat, args);
        va_end(args);
    }

    // 确保以'\n'结尾
    {
        size_t szTotal = strlen(cBuffer);
        if ((szTotal > 0) && (cBuffer[szTotal - 1] != '\n'))
        {
            if (szTotal < sizeof(cBuffer) - 2)
            {
                cBuffer[szTotal]     = '\n';
                cBuffer[szTotal + 1] = '\0';
            }
        }
    }

    g_pfnOutput(cBuffer);
}
