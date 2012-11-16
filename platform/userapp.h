#ifndef _MAIN_H_
#define _MAIN_H_

/*
 * 本头文件为用户应用程序需唯一包含的平台头文件。
 * 用户需实现本文件中声明的变量和函数。
 * 创建：2012-11-16 14:18:35
 * 作者：bbdlg
 */

#include <stdio.h>
#include <errModule.h>
#include <commModule.h>
#include <lgModule.h>
#include <logModule.h>
#include <tools.h>
#include <main.h>


/* 应用程序版本信息 */
extern char* appVerInfo;

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
extern void checkEvent(const timeval curTimeval);

#endif

