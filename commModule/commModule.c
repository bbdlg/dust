#include "commModule.h"

void* gLinkMap = NULL;
const char* defaultConfigFilePath = "./commModule.conf";

int getSizeOfGLinkMap(void)
{
   int sumOfMap = *((int*)gLinkMap);
   int ret = sizeof(int);
   while(sumOfMap--) {
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
#define MsumOfMap                   gLinkMap
#define MbaseMap(sizeOfGLinkMap)    (MsumOfMap + sizeOfGLinkMap)
#define MpLogicName(x)              MbaseMap(x)
#define MtypeOfMap(x)               (MpLogicName(x) + sizeof(char*))
#define MpMapLinkInfo(x)            (MtypeOfMap(x) + sizeof(int))
#define MsumOfFd(x)                 (MpMapLinkInfo(x) + sizeof(int*))
#define MbasePoolOfFd(x)            (MsumOfFd(x) + sizeof(int))
#define MinitPoolOfFd(x)            memset(MbasePoolOfFd(x), -1, (*(int*)MsumOfFd(x) * sizeof(int)))

#ifdef TCP_CLIENT_MODE
int initTcpClientInfo(const char* configFilePath)
{
   int sizeRes = MAX_LEN_VALUE;
   char res[MAX_LEN_VALUE] = {0};
   int sumTcpClient = readValueFromConf_ext(configFilePath, 0, "TcpClient", "anything", res, &sizeRes);
   //printf("sumTcpClient is <%d>, config file path is <%s>\n", sumTcpClient, configFilePath);
   while(sumTcpClient--) {
      char* logicName = NULL;
      TcpClientInfoObject* tcpClientInfoObject = (TcpClientInfoObject*)malloc(sizeof(TcpClientInfoObject));

      //read value from config file
      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "LogicName", res, &sizeRes)) {
         printf("error: can't get LogicName in %d lines!\n", __LINE__);
         continue;
      }
      logicName = (char*)malloc(sizeRes+1);
      memcpy(logicName, res, sizeRes);
      logicName[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpClient+1, "TcpClient", "DestIp", res, &sizeRes)) {
         printf("error: can't get DestIp %d lines!\n", __LINE__);
         continue;
      }
      tcpClientInfoObject->destIp = (char*)malloc(sizeRes+1);
      //DEBUG("IP p=%p,<%s>len<%d>", tcpClientInfoObject->destIp, res, sizeRes);
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
      gLinkMap = realloc(gLinkMap, (getSizeOfGLinkMap() + 4*sizeof(int) + 1*sizeof(int)));
      int x = getSizeOfGLinkMap();
      *((char**)MpLogicName(x))                   = logicName;
      *((int*)MtypeOfMap(x))                      = TCPCLIENT;
      *((TcpClientInfoObject**)MpMapLinkInfo(x))  = tcpClientInfoObject;
      *((int*)MsumOfFd(x))                        = 1;  //sum of tcp client is 1
      MinitPoolOfFd(x);
      *((int*)MsumOfMap)                          = *((int*)MsumOfMap) + 1;
      
      //debug
      //DEBUGINFO("size of glinkemap is %d", getSizeOfGLinkMap());
      int _i=0;
      for(_i=0; _i<64; _i++) {
         //printf("%02X ", *(unsigned char*)(gLinkMap+_i));
      }
      //DEBUGINFO();
   }

   return COMM_SUCCESS;
}

int connectTcpClient(int baseOfMap)
{
   TcpClientInfoObject* tcpClientInfoObject = *(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap);
   DEBUG("dest ip is %s,%d,%d", tcpClientInfoObject->destIp, tcpClientInfoObject->destPort, tcpClientInfoObject->localPort);
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(-1 == sockfd) {
      printf("create socket fd error!\n");
      return COMM_CREATE_FD_ERROR;
   }
   struct sockaddr_in serv_addr;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port   = htons(tcpClientInfoObject->destPort);
   serv_addr.sin_addr.s_addr = inet_addr(tcpClientInfoObject->destIp);
   bzero(&(serv_addr.sin_zero), 8);
   if (-1 == connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) {
      printf("connect error!\n");
      return COMM_CONNECT_FAILED;
   }

   *((int*)MbasePoolOfFd(baseOfMap)) = sockfd;
   DEBUG("connect success! <%s : %d>", tcpClientInfoObject->destIp, tcpClientInfoObject->destPort);

   return COMM_SUCCESS;
}
#endif

int initComm(const char* configFilePath)
{
   const char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;
   gLinkMap = malloc(sizeof(int));
   *(int*)MsumOfMap = 0;

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

   /* connect all links */
   int _sumOfMap = *(int*)MsumOfMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      switch(*(int*)MtypeOfMap(_curMapStart)) {
#ifdef TCP_CLIENT_MODE
         case TCPCLIENT:
            connectTcpClient(_curMapStart);
            break;
#endif
#ifdef TCP_SERVER_MODE
         case TCPSERVER:
            connectTcpServer(_curMapStart);
            break;
#endif
#ifdef UDP_MODE
         case UDP:
            connectUdp(_curMapStart);
            break;
#endif
         default:
            printf("unknown map type<%d>\n", *(int*)MtypeOfMap(_curMapStart));
            break;
      }
      
      //update _curMapStart
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return COMM_SUCCESS;
}

int commRecv(int fd, char* recvBuf, int* recvLen)
{
#ifdef TCP_CLIENT_MODE
   *recvLen = recv(fd, recvBuf, *recvLen, MSG_DONTWAIT);
   
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
   send(fd, sendBuf, sendLen, MSG_DONTWAIT);
#endif
#ifdef TCP_SERVER_MODE
   //do something
#endif
#ifdef UDP_MODE
   //do something
#endif

   return COMM_SUCCESS;
}

int getAliveLink(const char* logicName, int* sumFd, int** pFd)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      if(0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName)) {
         *sumFd = *(int*)MsumOfFd(_curMapStart);
         *pFd = (int*)MbasePoolOfFd(_curMapStart);
         return COMM_SUCCESS;
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return COMM_INVALID_LOGICNAME;
}

