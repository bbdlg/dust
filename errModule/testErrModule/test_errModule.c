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

#include "../errModule.h"
//#include "../../logModule/logModule.h"
#include "unity.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_moduleErrInfo(void)
{
   enum logErrNo { EA, EB, EC, logMAXERRNO };
   char* logErrInfo[] = {"error a", "error b", "error c"};
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, -1));
   TEST_ASSERT_EQUAL_STRING("error a", moduleErrInfo(log, 0));
   TEST_ASSERT_EQUAL_STRING("error b", moduleErrInfo(log, EB));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO+1));
   //log(LOG_DEBUG, "module:<%s>, errno:<%d>, msg:<%s>", "log", EA, moduleErrInfo(log, EA));
}
