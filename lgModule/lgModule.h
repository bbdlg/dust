#ifndef _LGMODULE_H_
#define _LGMODULE_H_
/****************************************************************************
 * name:     lg模块
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    通过TCP或UDP接收来自lp模块的“查岗”，可通过自定义命令处理函数完成展示当前
 *    程序运行状态、改变程序运行模式等功能。
 *    
 * 使用方法：
 *    1、lg和lp间商定自定义命令，如 show our_love_memory；
 *    2、通过调用addCmdFunction，注册show命令，及相应的处理函数 
 *          addCmdFunction(&procCmd_show, "show");
 *    3、在procCmd_show函数中完成对our_love_memory的处理：
 *          CmdFunction procCmd_show(int argc, char* argv[]) {
 *             term("we are both born in %d~", 1986);
 *          }
 *    4、启动lp.sh或用nc命令，发送一句"show our_love_memory"，lg就会应答一句：
 *          we are both born in 1986~
*****************************************************************************/

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include "main.h"

/*
 * Version and Compile Info
 * */
extern const char* lgVersion;
extern const char* lgCompileDate;
extern const char* lgCompileTime;

#define MAX_CMD_FUNCTION   30
#define MAX_LEN_CMD_PROMPT 24
#define MAX_LEN_USER_MSG   1000
#define MAX_SUM_CMD        100

enum lgErrNo {
   LG_SUCCESS = 0,
   LG_NO_WELCOMEINFO,
   LG_OVERMUCH_CMDFUNCTION,
   LG_INVALID_PARA,
   lgMAXERRNO
};

typedef void CmdFunction(int argc, char* argv[]);
typedef struct {
   CmdFunction* func;
   const char* cmd;
}PoolCmdFunction;

extern char userMsg[MAX_LEN_CMD_PROMPT + MAX_LEN_USER_MSG];
extern PoolCmdFunction poolCmdFunction[MAX_CMD_FUNCTION];

extern int initLgModule(const char* welcomeInfo);
extern int addCmdFunction(CmdFunction function, const char* name);
#define term(format, ...) sprintf(userMsg+strlen(userMsg), format, ##__VA_ARGS__)
/*******************************************************************************
 * optional.
 * called when the connection is established.
 * it will show welcome info to lpModule, and give a prompt, like 'lpsay: '
********************************************************************************/
extern int welcomeLp(char* sendbuf, int* sendlen);
extern int procLgModule(const char* recvbuf, const int recvlen, char* sendbuf, int* sendlen);
extern int addDefaultCmdFunction(void);

#endif

