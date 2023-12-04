/*
 * @Author: panlou lou.pan@ecovacs.com
 * @Date: 2023-07-03 17:35:15
 * @LastEditors: panlou lou.pan@ecovacs.com
 * @LastEditTime: 2023-08-24 15:26:52
 * @FilePath: /aplog/include/macro.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#pragma once

#include "config.hxx"
#include "logger.h"
#include "loggingevent.h"
#include "appendfile.h"
#include "appendconsole.h"
#include "unistd.h"
#include "layout.h"
#include "threadpool.h"
#include "connection.h"
#include "transporttcp.h"
#include "connectionmanager.h"

aplog::stringPtr funformat(const char *format, ...); 


#define TRACE_LOG 0
#define DEBUG_LOG 1
#define INFO_LOG  2
#define WARN_LOG  3
#define ERROR_LOG 4
#define FATAL_LOG 5

#define APLOG_PRIN_TRACE(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, TRACE_LOG, false, __LINE__,  __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_PRIN_DEBUG(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, DEBUG_LOG, false, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_PRIN_INFO(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, INFO_LOG, false, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_PRIN_WARN(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, WARN_LOG, false, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_PRIN_ERROR(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, ERROR_LOG, false, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_PRIN_FATAL(logger, fmt, ...)         \
    APLOG_DO_PRINT(logger, FATAL_LOG, false, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)


#define APLOG_TIME_PRIN_TRACE(logger, fmt, ...)              \
    APLOG_DO_PRINT(logger, TRACE_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_TIME_PRIN_DEBUG(logger, fmt, ...)             \
    APLOG_DO_PRINT(logger, DEBUG_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_TIME_PRIN_INFO(logger, fmt, ...)              \
    APLOG_DO_PRINT(logger, INFO_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_TIME_PRIN_WARN(logger, fmt, ...)              \
    APLOG_DO_PRINT(logger, WARN_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_TIME_PRIN_ERROR(logger, fmt, ...)             \
    APLOG_DO_PRINT(logger, ERROR_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)
#define  APLOG_TIME_PRIN_FATAL(logger, fmt, ...)             \
    APLOG_DO_PRINT(logger, FATAL_LOG, true, __LINE__, __func__, __FILE__, fmt, ##__VA_ARGS__)

#define APLOG_DO_PRINT(logger, loglevel, needtime, line, func, file, fmt, ...)                          \
    do {                                                                                                \
        if (logger->getLogLevel() < loglevel)  {                                                        \
            aplog::stringPtr _data                                                                      \
                = funformat(fmt, ##__VA_ARGS__);                                                        \
            aplog::logeventPtr _logEvent                                                                \
                = std::make_shared<aplog::LogEvent>(_data, loglevel, file, func, line, needtime);       \
            logger->write2Log(_logEvent);                                                               \
        }                                                                                               \
    } while(0);                                                                                         \

    
    
