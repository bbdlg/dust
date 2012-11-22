#include "userapp.h"

/* 应用程序版本信息 */
const char* appVersion     = "1.0.0";
const char* appCompileDate = __DATE__;
const char* appCompileTime = __TIME__;

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
void initAll(void)
{
   return;
}

/* 该函数将在主循环中循环被调用 */
void checkEvent(const struct timeval curTimeval)
{
   return;
}

