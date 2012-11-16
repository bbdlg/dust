#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <unistd.h>
#include <../errModule/errModule.h>
#include <../commModule/commModule.h>
#include <../lgModule/lgModule.h>
#include <../logModule/logModule.h>
#include <../tools/tools.h>


typedef void ConfAnalyzeFunc(const char* pathConf);

#define ERRORTIP(...) do{ 

/***************** 全局变量 *********************/

/* 应用程序版本信息，用户实现 */
extern char* appVerInfo;

/***************** 函数声明区 *******************/
/* 初始化函数，用户实现 */
extern void initAll(void);

/* 该函数将在主循环中循环被调用，用户实现 */
extern void checkEvent(const timeval curTimeval);

/* 注册配置文件解析函数，可多次调用 */
extern int registerCommFunc(ConfAnalyzeFunc* func);
/* 注册数据处理回调函数，可多次调用 */
extern int registerCommFunc(const char* runPara, const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);
/* 注册外部命令响应函数，可多次调用 */
extern int addCmdFunction(CmdFunction function, const char* name);

#endif

