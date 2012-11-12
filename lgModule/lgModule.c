#include "lgModule.h"

static const char* lgWelcomeInfo = "WELCOME TO LGMODULE";
static const char* defaultNoCmdPrompt = "no such command";
static char* defaultPrompt = "lpsay: ";
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
      argv[argc] = (char*)malloc(pSpace - lastpSpace + 1);
      memcpy(argv[argc], lastpSpace, pSpace-lastpSpace);
      //*(argv[argc] + strlen(argv[argc])) = 0; //the postion is not '\0', so can not use strlen().
      *(argv[argc] + (pSpace - lastpSpace)) = 0;
      argc++;
      lastpSpace = pSpace+1;
   }
   argv[argc] = (char*)malloc(recvbuf + recvlen - lastpSpace + 1);
   memcpy(argv[argc], lastpSpace, (recvbuf + recvlen - lastpSpace));
   *(argv[argc] + strlen(argv[argc])) = 0;
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
   if(0 == strlen(userMsg)) {
      printf("userMsg is empty\n");
   }
   else {
      printf("userMsg is not empty, <%s>\n", userMsg);
      strcat(userMsg, "\n");
   }
   memcpy(userMsg+strlen(userMsg), defaultPrompt, strlen(defaultPrompt));

   //sendbuf = userMsg;
   memcpy(sendbuf, userMsg, strlen(userMsg));
   *sendlen = strlen(userMsg);
   
   //empty the global array userMsg[]
   memset(userMsg, 0, sizeof(userMsg)/sizeof(userMsg[0]));

   return LG_SUCCESS;
}



