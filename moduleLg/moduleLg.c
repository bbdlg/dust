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

#include "moduleLg.h"

const char* lgVersion = "1.0.0";
const char* lgCompileDate = __DATE__;
const char* lgCompileTime = __TIME__;

static const char* lgWelcomeInfo = "WELCOME TO LGMODULE";
static const char* defaultNoCmdPrompt = "no such command";
static char* defaultPrompt = "lpsays>> ";
char userMsg[MAX_LEN_CMD_PROMPT + MAX_LEN_USER_MSG];

const char* lgErrInfo[] = {
   "success",
   "welcome info is null, now it will be the default one",
   "there's no any cmd function handler"
   "add too much function",
   "invalid input parameter",
};
PoolCmdFunction poolCmdFunction[MAX_CMD_FUNCTION] = {{NULL, NULL}};

int initLgModule(const char* welcomeInfo)
{
   int i=0;
   if(NULL == welcomeInfo) {
      return LG_NO_WELCOMEINFO;
   }
   lgWelcomeInfo = welcomeInfo;

   for(i=0; i<sizeof(poolCmdFunction)/sizeof(poolCmdFunction[0]); i++) {
      poolCmdFunction[i].func = NULL;
      poolCmdFunction[i].cmd  = NULL;
   }

   return LG_SUCCESS;
}

int addCmdFunction(CmdFunction function, const char* name)
{
   //debug
   //printf("sizeof(pool):<%d>, sizeof(CmdFunction):<%d>\n", sizeof(poolCmdFunction), sizeof(CmdFunction));

   int i=0;
   for(i=0; 1; i++) {
      if(i >= sizeof(poolCmdFunction)/sizeof(poolCmdFunction[0])) {
         return LG_OVERMUCH_CMDFUNCTION;
      }else if(NULL == poolCmdFunction[i].func) {
         poolCmdFunction[i].func = (CmdFunction*)function;
         poolCmdFunction[i].cmd = name;
         break;
      }
   }

   return LG_SUCCESS;
}

int welcomeLp(char* sendbuf, int* sendlen)
{
   term("%s", lgWelcomeInfo);
   strcat(userMsg, "\n");
   strcat(userMsg, defaultPrompt);
   
   //sendbuf = userMsg;
   memcpy(sendbuf, userMsg, strlen(userMsg));
   *sendlen = strlen(userMsg);
   
   //empty the global array userMsg[]
   memset(userMsg, 0, sizeof(userMsg)/sizeof(userMsg[0]));

   return LG_SUCCESS;
}

int procLgModule(const char* recvbuf, const int recvlen, char* sendbuf, int* sendlen)
{
   int i=0;   
   char* argv[MAX_SUM_CMD] = {0};
   int argc = 0;
   char* pSpace = NULL;
   char* lastpSpace = (char*)recvbuf;

   if((NULL == recvbuf) || (0 >= recvlen)) {
      return LG_INVALID_PARA;
   }

   while((pSpace = strstr(lastpSpace, " "))) {
      if((pSpace - lastpSpace) > recvlen) {
         break;
      }
      argv[argc] = (char*)malloc(pSpace - lastpSpace + 1);
      memcpy(argv[argc], lastpSpace, pSpace-lastpSpace);
      //*(argv[argc] + strlen(argv[argc])) = 0; //the postion is not '\0', so can not use strlen().
      *(argv[argc] + (pSpace - lastpSpace)) = 0;
      argc++;
      lastpSpace = pSpace+1;
   }
   argv[argc] = (char*)malloc(recvbuf + recvlen - lastpSpace + 1);
   memcpy(argv[argc], lastpSpace, (recvbuf + recvlen - lastpSpace));
   *(argv[argc] + (recvbuf + recvlen - lastpSpace)) = 0;
   //avoid the last argv tail with '\n'
   if('\n' == *(argv[argc] + strlen(argv[argc]) - 1)) {
      *(argv[argc] + strlen(argv[argc]) - 1) = 0;
   }
   argc++;

   for(i=0; i<sizeof(poolCmdFunction)/sizeof(poolCmdFunction[0]); i++) {
      if(NULL == poolCmdFunction[i].cmd) {
         continue;
      }
      if(0 == strcmp(argv[0], poolCmdFunction[i].cmd)) {
         break;
      }
   } 
   if(i < sizeof(poolCmdFunction)/sizeof(poolCmdFunction[0])) {
      if(poolCmdFunction[i].func) {
         (*poolCmdFunction[i].func)(argc, argv);
         for(i=0; i<argc; i++) {
            free(argv[i]);
            argv[i] = NULL;
         }
      }else {
         printf("can't find func handler, but this will be occur for ever\n");
      }
   }else if(strlen(argv[0])) {
      term("%s", defaultNoCmdPrompt);
   }

   if(strlen(userMsg)) {
      strcat(userMsg, "\n\n");
   }
   memcpy(userMsg+strlen(userMsg), defaultPrompt, strlen(defaultPrompt));

   //sendbuf = userMsg;
   memcpy(sendbuf, userMsg, strlen(userMsg));
   *sendlen = strlen(userMsg);
   
   //empty the global array userMsg[]
   memset(userMsg, 0, sizeof(userMsg)/sizeof(userMsg[0]));

   return LG_SUCCESS;
}

void lgCmdFuncHelp(int argc, char* argv[])
{
   int i=0;
   term("Enter the following commands:");
   for(i=0; i<MAX_CMD_FUNCTION; i++) {
      if(poolCmdFunction[i].cmd) {
         if(0 == i%3) term("\n");
         term("%10s", poolCmdFunction[i].cmd);
      }
   }
   return;
}

#ifndef WIN32
void lgCmdFuncUnix(int argc, char* argv[])
{
   char cmd[200] = {0};
   int i=0;
   char recv[1024] = {0};
   int size = sizeof(recv)/sizeof(recv[0]);
   if(1 == argc) {
      term("Usage: unix [shell command like 'ls']");
   }
   for(i=1; i<argc; i++) {
      if((strlen(cmd)+strlen(argv[i])) > sizeof(cmd)/sizeof(cmd[0])) {
         term("too long unix cmd");
         return;
      }
      sprintf(cmd+strlen(cmd), "%s ", argv[i]);
   }
      
   getResultFromSystemCall(cmd, recv, &size);
   if(size > 0) {
      term("%s", recv);
   }
   return;
}
#endif

void lgCmdFuncWelcome(int argc, char* argv[])
{
   term("%s", lgWelcomeInfo);
   return;
}

void lgCmdFuncAbout(int argc, char* argv[])
{
   term("Author: <bbdlg>\nThank my dear @quan-quan007:)");
   return;
}

void lgCmdFuncVersion(int argc, char* argv[])
{
#ifdef WIN32
   char* ver = "Unknown Compiler Version";
   if(1700 <  _MSC_VER) ver = "The compiler is too new, so i don't know";
   if(1700 == _MSC_VER) ver = "Visual Studio 2011";
   if(1600 == _MSC_VER) ver = "Visual Studio 2010";
   if(1500 == _MSC_VER) ver = "Visual Studio 2008";
   if(1400 == _MSC_VER) ver = "Visual Studio 2005";
   if(1310 == _MSC_VER) ver = "Visual Studio 2003";
   if(1300 == _MSC_VER) ver = "Visual Studio 2002";
   if(1300 >  _MSC_VER) ver = "The compiler is too old, so i don't know";
   printf("Compiler: %s\n", ver);
#else
   term("gcc: %s\n", __VERSION__);
#endif
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "app",   moduleVersion(app),  moduleCompileDate(app),    moduleCompileTime(app) );
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "comm",  moduleVersion(comm), moduleCompileDate(comm),   moduleCompileTime(comm));
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "log",   moduleVersion(log),  moduleCompileDate(log),    moduleCompileTime(log));
   term("module: %s, \tversion: %s, \tcompile: %s %s  ", "lg",    moduleVersion(lg),   moduleCompileDate(lg),     moduleCompileTime(lg));
}

int addDefaultCmdFunction(void)
{
   addCmdFunction(&lgCmdFuncHelp,      "help");
#ifndef WIN32
   addCmdFunction(&lgCmdFuncUnix,      "unix");
#endif
   addCmdFunction(&lgCmdFuncWelcome,   "welcome");
   addCmdFunction(&lgCmdFuncAbout,     "about");
   addCmdFunction(&lgCmdFuncVersion,   "version");
   addCmdFunction(&lgCmdFuncTrace,     "trace");

   return LG_SUCCESS;
}

