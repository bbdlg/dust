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

#ifndef _ERRMODULE_H_
#define _ERRMODULE_H_
/****************************************************************************
 * name:     error manage module
 * version:  v1.0.0
 * time:     2012-11-26 20:47:08
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * function:
 *    Provide a public interface to get module's inside error info via module 
 *    name and error code.
 *    Provide function of detecting illegal error code.
 *
 * Use-method:
 *    1. Include this file.
 *    2. In other module header file define <module's inside error code>:
 *       enum modulenameErrNo { 
 *          EA = 0, 
 *          EB, 
 *          EC, 
 *          //..., 
 *          modulenameMAXERRNO };
 *    3. In other module source file define <module's inside error info>:
 *       const char* modulenameErrInfo[modulenameMAXERRNO] = { 
 *          "error a", 
 *          "error b", 
 *          "error c", 
 *          //... };
 *    4. Use moduleErrInfo(modulename, ErrNo) to get corresponding error info,
 *       among ErrNo is the error code defined in modulenameErrNo.
 *
 * E.g.
 *    Assuming module name is 'log', define the following error code and error
 *    info inside module:
 *       enum logErrNo { EA=0, EB, EC, logMAXERRNO };
 *       const char* logErrInfo[] = {"error a", "error b", "error c" };
 *    We can get the error info 'error b' of error code 'EB':
 *       moduleErrInfo(log, EB);
 *
*****************************************************************************/

#define joinModuleErrInfo(moduleName)     (moduleName##ErrInfo)
#define moduleMaxErrNo(moduleName)        (moduleName##MAXERRNO)
#define moduleErrInfo(moduleName, errno)  ((errno<0) || (errno>=moduleMaxErrNo(moduleName))) ? \
                                             "unknown "#moduleName" module error" : \
                                             joinModuleErrInfo(moduleName)[errno]
#define moduleVersion(moduleName)         (moduleName##Version)
#define moduleCompileDate(moduleName)     (moduleName##CompileDate)
#define moduleCompileTime(moduleName)     (moduleName##CompileTime)

#endif

