#ifndef _LOGMODULE_H_
#define _LOGMODULE_H_
/****************************************************************************
 * name:     日志记录模块
 * version:  v1.0
 * time:     2012-9-26 16:04:37
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    提供四个级别的定义（DEBUG、INFO、WARNING、ERROR）；
 *    在给定路径（root_log_path）下记录日志；
 *    日志格式为 root_log_path/yyyymmdd/hhmmssuuuuuu.log；
 *    可按时间记录或日志文件大小切换（时间及大小可配）；
 *    日志记录路径、文件切换时间、文件切换大小三个参数可通过调用 initLog() 
 *    或在当前目录下的 logModule.conf 中配置。
 * 使用方法：
 *    1、通过调用initLog()或在当前目录下的 logModule.conf 中配置好
 *       日志记录路径、文件切换时间、文件切换大小三个参数，
 *       或直接使用默认参数 "/tmp/log", 60s, 1024kb；
 *    2、调用log宏写日志。log的用法如：
 *          log(DEBUG, "%d-%s", 123, "this is a test message~");
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "tools.h"
#ifdef LOG
   #include "errModule.h"
#endif

extern const char* logVersion;
extern const char* logCompileDate;
extern const char* logCompileTime;

/* Error in this module */
enum logErrNo {
   LOG_SUCCESS = 0,
   LOG_INVALID_INPUT_PARA,
   LOG_FP_IS_NULL,
   logMAXERRNO
}; 
extern const char* logErrInfo[logMAXERRNO];

extern int logInit(const char* configFilePath);

/* Call initLog() before call log to record your log. 
 * if you didn't do this, the log will be record into "/tmp/log" as default.
 * rootPathStoreLog appoints path to record log files, like "/tmp/log" or "/tmp/log/".
 * secondsSwitchLog appoints a period of switching log files by unit seconds, like 60.
 * kbSwitchLog appoints a max size of a single log file, unit is kilobyte.
 * */
extern int initLog(char* rootPathStoreLog, int secondsSwitchLog, int kbSwitchLog); 

/* You have four levels to log 
 * the max len of your single message is defined by MAX_LEN_MSG
 * call the macro log() to log, you can use it like printf. 
 * the difference between printf and log is log add a level before all parameter.
 * example: 
 *    log(LOG_INFO, "i have %d kinds of fruit, %s is one of them\n", 4, apple);
 * */
enum LogLevel {
   LOG_DEBUG,     //print on screen only
   LOG_INFO,      //print on screen and write in file
   LOG_WARNING,   //print on screen and write in file
   LOG_ERROR      //print on screen and write in file
};
extern const char* logLevelStr[];
#define MAX_LEN_MSG   1000
#define log(level,format,...)  do{ \
   char _msg[MAX_LEN_MSG] = {0}; \
   sprintf(_msg, format, ##__VA_ARGS__); \
   _log(level, __FUNCTION__, __FILE__, __LINE__, _msg); \
}while(0)


/* Call destoryLog() when you don't want to log anymore */
extern int destoryLog(void);


/* Only call by log.c and testfiles*/
#ifndef MAX_LEN_PATH 
   #define MAX_LEN_PATH    256
#endif
extern char _rootPathStoreLog[MAX_LEN_PATH];
extern int _secondsSwitchLog;
extern int _kbSwitchLog;

#endif

