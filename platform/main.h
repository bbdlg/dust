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

