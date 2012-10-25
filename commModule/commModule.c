#include "commModule.h"

void* gLinkMap = NULL;
const char* defaultConfigFilePath = "./commModule.conf";

int getSizeOfGLinkMap(void)
{
   int sumOfMap = *(int*)gLinkMap;
   int ret = sizeof(int);
   while(sumOfMap--) {
      printf("_____________________________\n");
      fflush(stdout);
      ret += sizeof(char*);
      ret += sizeof(int);
      ret += sizeof(int*); //equal to TcpClientInfoObject*
      int sumOfFd = *((int*)(gLinkMap + ret));
      ret += sizeof(int);
      ret += (sizeof(int) * sumOfFd);
   }
   return ret;
}

#define MAX_LEN_VALUE 100
#define MsumOfMap     (int*)gLinkMap
#define MbaseMap(sizeOfGLinkMap)     (gLinkMap + sizeOfGLinkMap)
#define MpLogicName(x)   (char*)MbaseMap(x)
#define MtypeOfMap(x)    ((int*)(MpLogicName(x) + sizeof(MpLogicName(x))))
#define MpMapLinkInfo(x) ((int*)(MtypeOfMap(x) + sizeof(MtypeOfMap(x))))
#define MsumOfFd(x)      ((int*)(MpMapLinkInfo(x) + sizeof(MpMapLinkInfo(x))))
#define MbasePoolOfFd(x) ((int*)(MsumOfFd(x) + sizeof(MsumOfFd(x))))
#define MinitPoolOfFd(x) memset(MbasePoolOfFd(x), (int)-1, sizeof(int)*(*MsumOfFd(x)))

#ifdef TCP_CLIENT_MODE
int initTcpClientInfo(const char* configFilePath)
{
   int sizeRes = MAX_LEN_VALUE;
   char res[MAX_LEN_VALUE] = {0};
   int sumTcpClient = readValueFromConf_ext(configFilePath, 0, "TcpClient", "anything", res, &sizeRes);
   printf("sumTcpClient is <%d>, config file path is <%s>\n", sumTcpClient, configFilePath);
   while(sumTcpClient--) {
      char* logicName = (char*)malloc(sizeRes+1);
      TcpClientInfoObject* tcpClientInfoObject = (TcpClientInfoObject*)malloc(sizeof(TcpClientInfoObject));

      //read value from config file
      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "LogicName", res, &sizeRes)) {
         printf("error: can't get LogicName in %d lines!\n", __LINE__);
         continue;
      }
      memcpy(logicName, res, sizeRes);
      logicName[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "DestIp", res, &sizeRes)) {
         printf("error: can't get DestIp %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->destIp = (char*)malloc(sizeRes+1);
      memcpy(tcpClientInfoObject->destIp, res, sizeRes);
      tcpClientInfoObject->destIp[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "DestPort", res, &sizeRes)) {
         printf("error: can't get DestPort in %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->destPort = atoi(res);

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "LocalPort", res, &sizeRes)) {
         printf("error: can't get LocalPort in %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->localPort = atoi(res);
      
      //fill gLinkMap
      realloc(gLinkMap, (getSizeOfGLinkMap() + (1+1+1+1+1)*4));
      *MsumOfMap++;
      int x = getSizeOfGLinkMap();
      *MpLogicName(x)   = logicName;
      *MtypeOfMap(x)    = TCPCLIENT;
      *MpMapLinkInfo(x) = tcpClientInfoObject;
      *MsumOfFd(x)      = 1;  //sum of tcp client is 1
      MinitPoolOfFd(x);
      
      //debug
      printf("sizeOfGLinkMap is <%d>[%d]lines\n", getSizeOfGLinkMap(), __LINE__);
      fflush(stdout);
   }

   return COMM_SUCCESS;
}
#endif

int initComm(const char* configFilePath)
{
#ifdef MAX_LEN_VALUE
#pragma message("hallo, initComm")
#endif
   const char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;
   gLinkMap = malloc(sizeof(int));
   *MsumOfMap = 0;

   /* load config info */
#ifdef TCP_CLIENT_MODE
#pragma message("hallo, tcp client mode")  
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

