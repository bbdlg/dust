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

#include "main.h"

char* homePath = NULL;

void procLpMsg(const char* logicName, const int fd, const char* recvbuf, const int recvlen, void* from)
{
   char sendbuf[1024] = {0};
   int sendlen = 0;
   procLgModule(recvbuf, recvlen, sendbuf, &sendlen);
   commSend(fd, sendbuf, &sendlen, from);
}

void showVersion(void)
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
   printf("gcc: %s\n", __VERSION__);
#endif
   printf("module: %s, \tversion: %s, \tcompile: %s %s\n", "app",   moduleVersion(app),  moduleCompileDate(app),    moduleCompileTime(app) );
   printf("module: %s, \tversion: %s, \tcompile: %s %s\n", "comm",  moduleVersion(comm), moduleCompileDate(comm),   moduleCompileTime(comm));
   printf("module: %s, \tversion: %s, \tcompile: %s %s\n", "log",   moduleVersion(log),  moduleCompileDate(log),    moduleCompileTime(log));
   printf("module: %s, \tversion: %s, \tcompile: %s %s\n", "lg",    moduleVersion(lg),   moduleCompileDate(lg),     moduleCompileTime(lg));
   printf("\n");
}

void handlerSigInt(int sig)
{
   printf("recv SIGINT <%d>, you may press ctrl+C.\n", sig);
   commFreeAllFds();
#ifdef WIN32
   Sleep(1000);
#else
   sleep(1);
#endif
   exit(1);
}

void handlerSigSegv(int sig)
{
   printf("recv SIGSEGV <%d>, segment fault.\n", sig);
   commFreeAllFds();
#ifdef WIN32
   Sleep(1000);
#else
   sleep(1);
#endif
   exit(1);
}

int main(int argc, char** argv)
{
   int ret,i;
   char tmp[256] = {0};
   struct timeval curTimeval;
   const char* subdir[] = {"", "bin", "conf", "log", "data"};
   char mkdirCmd[256] = {0};

   //setup signal process handler
   signal(SIGINT,  handlerSigInt);
   signal(SIGSEGV, handlerSigSegv);

   //show version
   if(argv[1] && (0 == strcmp(argv[1], "-v"))) {
      showVersion();
      exit(1);
   }

   //check $DFCHOME
   memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
   homePath = getenv("DFCHOME");
   if(NULL == homePath) {
      ERRORTIP("$DFCHOME is null, try export...");
   }

   //init logModule
   memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
   sprintf(tmp, "%s/conf/dfc.conf", homePath);
   ret = logInit(tmp);
   if(ret) {
      ERRORTIP("logInit-%s", moduleErrInfo(log, ret));
   }
   log(LOG_INFO, "App<%s> start running!", moduleVersion(app));
   log(LOG_INFO, "Log file will be in <%s/log/>", getenv("DFCHOME"));

   //check running environment
   if(NULL == moduleVersion(app)) {
      log(LOG_WARNING, "it seems that appVerInfo is not set");
   }
   if(NULL == homePath) {
      log(LOG_ERROR, "DFCHOME is not set");
      exit(1);
   }
   if(strlen(homePath) > 200) {
      log(LOG_ERROR, "$DFCHOME is too long");
      exit(1);
   }
   for(i=0; i<sizeof(subdir)/sizeof(subdir[0]); i++) {
      memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
      sprintf(tmp, "%s/%s", homePath, subdir[i]);
      if(access(tmp, 0)) {
         log(LOG_WARNING, "%s does not exist, now create it...", tmp);
         memset(mkdirCmd, 0, sizeof(mkdirCmd)/sizeof(mkdirCmd[0]));
         sprintf(mkdirCmd, "mkdir %s", tmp);
         system(mkdirCmd);
      }
   }
   
   //init commModule
   memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
   sprintf(tmp, "%s/conf/dfc.conf", homePath);
   ret = commInit(tmp);
   if(ret) {
      log(LOG_ERROR, "commInit %s", moduleErrInfo(comm, ret));
      exit(1);
   }
   ret = commConnect(NULL);
   if(ret) {
      log(LOG_ERROR, "commConnect %s", moduleErrInfo(comm, ret));
      //exit(1);
   }

   //init lgModule
   initLgModule("*** HUANYING WO DE QINQINBAO ***\n  Enter 'help' to get help msg~\n-----------------------------------");
   addDefaultCmdFunction();
   ret = commSetFunc("dustLgModule", (RegisterFunc*)&welcomeLp, &procLpMsg);
   if(ret) {
      log(LOG_ERROR, "it seems that logicName<dustLgModule> is not set");
      //exit(1);
   }
 
   //init every function
   initAll();

   //step into endless loop
   while(1) {
      gettimeofday(&curTimeval, NULL);
      checkEvent(curTimeval);
      commProcess(curTimeval);
   }

   return 0;
}

