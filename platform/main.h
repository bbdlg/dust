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
 * name:     platform main source code
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * function:
 *    User needs to achieve all global variables and functions in this header file.
 *    
*****************************************************************************/

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <stdio.h>
//#include <time.h>
#include "moduleTools.h"
#include "moduleErr.h"
#include "moduleComm.h"
#include "moduleLg.h"
#include "moduleLog.h"

/* Application Version Information */
extern const char* appVersion;
extern const char* appCompileDate;
extern const char* appCompileTime;

/*
 * Initialization Function
 * Call the following functions in this function:
 * register config file analyze function, can be called repeatedly.
 *    extern int registerCommFunc(ConfAnalyzeFunc* func);
 * register data process callback function, can be called repeatedly.
 *    extern int registerCommFunc(const char* runPara, const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);
 * register outside order response function, can be called repeatedly.
 *    extern int addCmdFunction(CmdFunction function, const char* name);
 */
extern void initAll(void);

/* Periodic Callback Function */
extern void checkEvent(const struct timeval curTimeval);

#endif

