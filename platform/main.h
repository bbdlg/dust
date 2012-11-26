#ifndef _MAIN_H_
#define _MAIN_H_
/****************************************************************************
 * name:     平台主函数源码
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    用户需要实现本头文件中的全局变量及函数。
 *    
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "errModule.h"
#include "commModule.h"
#include "lgModule.h"
#include "logModule.h"
#include "toolsModule.h"

/* 应用程序版本信息 */
extern const char* appVersion;
extern const char* appCompileDate;
extern const char* appCompileTime;

/*
 * 初始化函数
 * 可在本函数中调用下列函数：
 * 注册配置文件解析函数，可多次调用
 * extern int registerCommFunc(ConfAnalyzeFunc* func);
 * 注册数据处理回调函数，可多次调用
 * extern int registerCommFunc(const char* runPara, const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);
 * 注册外部命令响应函数，可多次调用
 * extern int addCmdFunction(CmdFunction function, const char* name);
 */
extern void initAll(void);

/* 该函数将在主循环中循环被调用 */
extern void checkEvent(const struct timeval curTimeval);

#endif

