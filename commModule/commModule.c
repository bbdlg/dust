#include "commModule.h"

void* gLinkMap = NULL;
const char* defaultConfigFilePath = "./commModule.conf";

int initComm(const char* configFilePath)
{
   char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;

   //load config info
#ifdef TCP_CLIENT_MODE
   //do something
#endif
#ifdef TCP_SERVER_MODE
   //do something
#endif
#ifdef UDP_MODE
   //do something
#endif

   return COMM_SUCCESS;
}

int commRecv(int fd, char* recvBuf, int* recvLen)
{
#ifdef TCP_CLIENT_MODE
   //do something
#endif
#ifdef TCP_SERVER_MODE
   //do something
#endif
#ifdef UDP_MODE
   //do something
#endif

   return COMM_SUCCESS;
}

int commSend(int fd, const char* sendBuf, int sendLen)
{
#ifdef TCP_CLIENT_MODE
   //do something
#endif
#ifdef TCP_SERVER_MODE
   //do something
#endif
#ifdef UDP_MODE
   //do something
#endif

   return COMM_SUCCESS;
}

int getAliveLink(const char* logicName, int* sumFd, int* pFd)
{
   return COMM_SUCCESS;
}

