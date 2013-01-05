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
 * name:     错误管理模块
 * version:  v1.0.0
 * time:     2012-11-26 20:47:08
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    提供了一个公共接口，用以根据模块名及错误代码获取模块内部定义的错误信息
 *    提供检测非法错误代码功能。
 *
 * 使用方法：
 *    1、在其他模块头文件中包含本文件；
 *    2、需在其他模块头文件中定义<模块内部错误代码>： 
 *       enum modulenameErrNo { 
 *          EA = 0, 
 *          EB, 
 *          EC, 
 *          //..., 
 *          modulenameMAXERRNO };
 *    3、需在其他模块源文件中定义<模块内部错误信息>： 
 *       const char* modulenameErrInfo[modulenameMAXERRNO] = { 
 *          "error a", 
 *          "error b", 
 *          "error c", 
 *          //... };
 *    3、使用 moduleErrInfo(modulename, ErrNo) 来获得相应的错误信息，
 *    其中ErrNo为modulenameErrNo中定义的错误代码。
 *
 * 示例：
 *    假设模块名称为log，在模块内部定义了如下的错误代码及错误信息：
 *       enum logErrNo { EA=0, EB, EC, logMAXERRNO };
 *       const char* logErrInfo[] = {"error a", "error b", "error c" };
 *    我们可以通过如下语句获得错误代码为‘EB’的错误信息‘error b’：
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

