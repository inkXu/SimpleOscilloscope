/*
 * author: inkXu
 * date:  2023.5.23
 *
 * log
 */
#ifndef _EM_LOG_H_
#define _EM_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define OPEN_ZLOG         1
#define OPEN_ZLOG_DISPLAY 1
#define OPEN_ZLOG_SAVE    0
#define LOG_LEVEL LOG_DEBUG

typedef enum{
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL,
}E_LOGLEVEL;

char * EM_LOGLEVELGet(const int level);

void EM_LOG(const int level, const char *func, const int line, const char *fmt, ...);

#define EMLOG(level, fmt...) EM_LOG(level, __FUNCTION__, __LINE__, fmt)

#define FILE_PATH "./log.txt"
#define FILE_MODE "a"
#define SAVELOG(logmessage) savelog(logmessage, FILE_PATH, FILE_MODE)

void savelog(const char *log_message, const char *file_path, const char *mode);

#ifdef __cplusplus
}
#endif

#endif
