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

#include "../lgModule.h"
#include "unity.h"

const char* appCompileTime;
const char* appCompileDate;
const char* appVersion;
const char* commCompileTime;
const char* commCompileDate;
const char* commVersion;
const char* logCompileTime;
const char* logCompileDate;
const char* logVersion;

void setUp(void)
{

}

void tearDown(void)
{
   memset(poolCmdFunction, 0, sizeof(poolCmdFunction));
}

void test_initLgModule(void)
{
   TEST_ASSERT_EQUAL_INT(LG_NO_WELCOMEINFO, initLgModule(NULL));
   //TEST_ASSERT_EQUAL_INT(LG_SUCCESS, initLgModule("welcome to test lgModule"));
}

void test_welcomeLp(void)
{
   char sendbuf[1024] = {0};
   int sendlen = 0;
   welcomeLp(sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("WELCOME TO LGMODULE\nlpsays>> ", sendbuf);
   
   initLgModule("HALLO, BB~");
   memset(sendbuf, 0, sizeof(sendbuf)/sizeof(sendbuf[0]));
   welcomeLp(sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("HALLO, BB~\nlpsays>> ", sendbuf);
}

void exampleCmdFunction(int argc, char* argv[])
{
   int i = argc;
   while(argc--) {
      term("%s", argv[argc]);
   }
}

void test_addCmdFunction(void)
{
   int i=0;
   for(i=0; i<MAX_CMD_FUNCTION; i++) {
      char ctmp[10]={0};
      sprintf(ctmp, "cmd%d", i);
      TEST_ASSERT_EQUAL_INT(LG_SUCCESS, addCmdFunction(exampleCmdFunction, ctmp));
   }
   TEST_ASSERT_EQUAL_INT(LG_OVERMUCH_CMDFUNCTION, addCmdFunction(exampleCmdFunction, "cmdn"));
}

void test_procLgModule(void)
{
   addCmdFunction(exampleCmdFunction, "testcmd");
   const char recvbuf[] = "testcmd 1 2 3 4t";
   char sendbuf[1024] = {0};
   int sendlen = 0;

   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("4t321testcmd\n\nlpsays>> ", sendbuf);

   memset(sendbuf, 0, sizeof(sendbuf)/sizeof(sendbuf[0]));
   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("4t321testcmd\n\nlpsays>> ", sendbuf);
}

void test_procLgModule_notAddFunc(void)
{
   const char recvbuf[] = "testcmd 1 2 3 4t";
   char sendbuf[1024] = {0};
   int sendlen = 0;
   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("no such command\n\nlpsays>> ", sendbuf);
}

