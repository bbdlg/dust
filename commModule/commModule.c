#include "commModule.h"

void* gLinkMap = NULL;
const char* defaultConfigFilePath = "./commModule.conf";

int getSizeOfGLinkMap(void)
{
   int sumOfMap = *(int*)gLinkMap;
   int ret = sizeof(int);
   while(sumOfMap--) {
      ret += sizeof(char*);
      ret += sizeof(int);
      ret += sizeof(int*); //equal to TcpClientInfoObject*
      int sumOfFd = *(int*)gLinkMap[ret];
      ret += sizeof(int);
      ret += (sizeof(int) * sumOfFd);
   }
   return ret;
}

#define MAX_LEN_VALUE 100
#define MsumOfMap     (int*)gLinkMap
#define MbaseMap      (gLinkMap + getSizeOfGLinkMap())
#define MpLogicName   (char*)MbaseMap
#define MtypeOfMap    (int*)(MpLogicName + sizeof(MpLogicName))
#define MpMapLinkInfo (int*)(MtypeOfMap + sizeof(MtypeOfMap))
#define MsumOfFd      (int*)(MpMapLinkInfo + sizeof(MpMapLinkInfo))
#define MbasePoolOfFd (int*)(MsumOfFd + sizeof(MsumOfFd))
#define MinitPoolOfFd memset(MbasePoolOfFd, -1, sizeof(int)*(*MsumOfFd))

int initTcpClientInfo(const char* configFilePath)
{
   int sizeRes = MAX_LEN_VALUE;
   char res[sizeRes] = {0};
   int sumTcpClient = readValueFromConf_ext(_configFilePath, 0, "TcpClient", "anything", res, &sizeRes);
   while(sumTcpClient--) {
      char* logicName = (char*)malloc(sizeRes+1);
      TcpClientInfoObject* tcpClientInfoObject = (TcpClientInfoObject*)malloc(sizeof(TcpClientInfoObject));

      //read value from config file
      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(_configFilePath, sumTcpClient+1, "TcpClient", "LogicName", res, &sizeRes)) {
         printf("error: can't get LogicName in %d lines!\n", __LINE__);
         continue;
      }
      memcpy(logicName, res, sizeRes);
      logicName[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(_configFilePath, sumTcpClient+1, "TcpClient", "DestIp", res, &sizeRes)) {
         printf("error: can't get DestIp %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->destIp = (char*)malloc(sizeRes+1);
      memcpy(tcpClientInfoObject->destIp, res, sizeRes);
      tcpClientInfoObject->destIp[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(_configFilePath, sumTcpClient+1, "TcpClient", "DestPort", res, &sizeRes)) {
         printf("error: can't get DestPort in %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->destPort = atoi(res);

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(_configFilePath, sumTcpClient+1, "TcpClient", "LocalPort", res, &sizeRes)) {
         printf("error: can't get LocalPort in %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->localPort = atoi(res);
      
      //fill gLinkMap
      realloc(gLinkMap, getSizeOfGLinkMap() + (1+1+1+1+1)*4);
      *MsumOfMap++;
      *MpLogicName   = logicName;
      *MtypeOfMap    = TCPCLIENT;
      *MpMapLinkInfo = tcpClientInfoObject;
      *MsumOfFd      = 1;  //sum of tcp client is 1
      MinitPoolOfFd();
   }

   return COMM_SUCCESS;
}

int initComm(const char* configFilePath)
{
   const char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;
   gLinkMap = malloc(sizeof(int));
   *MsumOfMap = 0;

   /* load config info */
#ifdef TCP_CLIENT_MODE
   initTcpClientInfo(_configFilePath);
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

