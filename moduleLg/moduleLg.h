/*
 * This file is part of the DFC.
 *
 * Copyright (C) 2012 by bbdlg <shiluo.110@163.com>
 *
 * The DFC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The DFC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DFC. If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef _LGMODULE_H_
#define _LGMODULE_H_
/****************************************************************************
 * name:     LG module
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * function:
 *    User can get program inside status or change program running mode via LP
 *    module.
 *    
 * Use-method:
 *    1. Set a user-defined order like: show our_love_memory;
 *    2. Register 'show' order and corresponding function via calling addCmdFunction:
 *          addCmdFunction(&procCmd_show, "show");
 *    3. Finish procession of 'our_love_memory' in procCmd_show():
 *          CmdFunction procCmd_show(int argc, char* argv[]) {
 *             term("we are both born in %d~", 1986);
 *          }
 *    4. Run lp.sh or use 'nc' order, send a string "show our_love_memory", LG module
 *       will respond with:
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

