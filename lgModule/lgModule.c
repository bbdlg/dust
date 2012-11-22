#include "lgModule.h"

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
   if(NULL == welcomeInfo) {
      return LG_NO_WELCOMEINFO;
   }
   lgWelcomeInfo = welcomeInfo;

   int i=0;
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
   if((NULL == recvbuf) || (0 >= recvlen)) {
      return LG_INVALID_PARA;
   }
   
   char* argv[MAX_SUM_CMD] = {0};
   int argc = 0;
   char* pSpace = NULL;
   char* lastpSpace = (char*)recvbuf;
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

   int i=0;
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
         term("%+10s", poolCmdFunction[i].cmd);
      }
   }
   return;
}

void lgCmdFuncUnix(int argc, char* argv[])
{
   if(1 == argc) {
      term("Usage: unix [shell command like 'ls']");
   }
   char cmd[200] = {0};
   int i=0;
   for(i=1; i<argc; i++) {
      if((strlen(cmd)+strlen(argv[i])) > sizeof(cmd)/sizeof(cmd[0])) {
         term("too long unix cmd");
         return;
      }
      sprintf(cmd+strlen(cmd), "%s ", argv[i]);
   }
      
   char recv[1024] = {0};
   int size = sizeof(recv)/sizeof(recv[0]);
   getResultFromSystemCall(cmd, recv, &size);
   if(size > 0) {
      term("%s", recv);
   }
   return;
}

void lgCmdFuncWelcome(int argc, char* argv[])
{
   term("%s", lgWelcomeInfo);
   return;
}

void lgCmdFuncAbout(int argc, char* argv[])
{
   term("Author <bbdlg>\nWrite this module for my dear @quan-quan007");
   return;
}

void lgCmdFuncVersion(int argc, char* argv[])
{
   term("gcc: %s\n", __VERSION__);
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "app",   moduleVersion(app),  moduleCompileDate(app),    moduleCompileTime(app) );
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "comm",  moduleVersion(comm), moduleCompileDate(comm),   moduleCompileTime(comm));
   term("module: %s, \tversion: %s, \tcompile: %s %s\n", "log",   moduleVersion(log),  moduleCompileDate(log),    moduleCompileTime(log));
   term("module: %s, \tversion: %s, \tcompile: %s %s  ", "lg",    moduleVersion(lg),   moduleCompileDate(lg),     moduleCompileTime(lg));
}

int addDefaultCmdFunction(void)
{
   addCmdFunction(&lgCmdFuncHelp,      "help");
   addCmdFunction(&lgCmdFuncUnix,      "unix");
   addCmdFunction(&lgCmdFuncWelcome,   "welcome");
   addCmdFunction(&lgCmdFuncAbout,     "about");
   addCmdFunction(&lgCmdFuncVersion,   "version");

   return LG_SUCCESS;
}

