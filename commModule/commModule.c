#include "commModule.h"

void* gLinkMap = NULL;
struct timeval timeOutOfSelect = {0, 100000}; //default timeout: 100ms
const char* defaultConfigFilePath = "./commModule.conf";

int setSocketNonBlock(int nSocketFd)
{
   int nOpts;

   if (nSocketFd <= 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }

   nOpts = fcntl(nSocketFd, F_GETFL);
   if (nOpts < 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }
   if (fcntl(nSocketFd, F_SETFL, nOpts | O_NONBLOCK) < 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }

   return COMM_SUCCESS;
}

int setSocketBlock(int nSocketFd)
{
   int nOpts;

   if (nSocketFd <= 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }

   nOpts = fcntl(nSocketFd, F_GETFL);
   if (nOpts < 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }
   if (fcntl(nSocketFd, F_SETFL, nOpts & ~O_NONBLOCK) < 0)
   {
      return COMM_SET_SOCKET_FAILED;
   }

   return COMM_SUCCESS;
}

void printGLinkMap(const char* logicName)
{
   printf("\n@@@@@@@@@@@@@@@@@@ Link Map @@@@@@@@@@@@@@@@@@@\n");
   int sumOfMap = *(int*)gLinkMap;
   int i=0;
   int cur=4;
   char* sortName[] = {"tcp client", "tcp server", "udp", "serial", "can",};
   for(i=0; i<sumOfMap; i++) {
      if(logicName && strcmp(logicName, *(char**)MpLogicName(cur))) {
         continue;
      }
      printf(">> %d << \"%.15s\" \"%.15s\" <%.3d>fds:[", i+1, *(char**)MpLogicName(cur), sortName[*(int*)MtypeOfMap(cur)], *(int*)MsumOfFd(cur));
      int j=0;
      for(j=0; j<*(int*)MsumOfFd(cur); j++) {
         printf("%06d ", *((int*)MbasePoolOfFd(cur) + j));
      }
      printf("]\n");
      cur += 4*(4 + *(int*)MsumOfFd(cur));
   }
   printf("\n\n");
   fflush(stdout);
}

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

      tcpClientInfoObject->state     = DISCONNECTED;
      
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
   if((CONNECTED == (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->state) 
         && (fdInitValue != *(int*)MbasePoolOfFd(baseOfMap))) {
      return COMM_SUCCESS;
   }
   TcpClientInfoObject* tcpClientInfoObject = *(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap);
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(0 > sockfd) {
      return COMM_CREATE_FD_ERROR;
   }
   struct sockaddr_in serverAddr;
   serverAddr.sin_family       = AF_INET;
   serverAddr.sin_port         = htons(tcpClientInfoObject->destPort);
   serverAddr.sin_addr.s_addr  = inet_addr(tcpClientInfoObject->destIp);
   bzero(&(serverAddr.sin_zero), 8);

   setSocketNonBlock(sockfd);

   if (-1 == connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr))) {
      printf("connect error!\n");
      return COMM_CONNECT_FAILED;
   }

   *((int*)MbasePoolOfFd(baseOfMap)) = sockfd;
   (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->state = CONNECTED;

   return COMM_SUCCESS;
}
#endif

#ifdef TCP_SERVER_MODE
int initTcpServerInfo(const char* configFilePath)
{
   int sizeRes = MAX_LEN_VALUE;
   char res[MAX_LEN_VALUE] = {0};
   int sumTcpServer = readValueFromConf_ext(configFilePath, 0, "TcpServer", "anything", res, &sizeRes);
   while(sumTcpServer--) {
      char* logicName = NULL;
      int maxLink = -1;
      TcpServerInfoObject* tcpServerInfoObject = (TcpServerInfoObject*)malloc(sizeof(TcpServerInfoObject));

      //read value from config file
      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpServer+1, "TcpServer", "LogicName", res, &sizeRes)) {
         printf("error: can't get LogicName in %d lines!\n", __LINE__);
         continue;
      }
      logicName = (char*)malloc(sizeRes+1);
      memcpy(logicName, res, sizeRes);
      logicName[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpServer+1, "TcpServer", "ServerPort", res, &sizeRes)) {
         printf("error: can't get ServerPort in %d lines!\n", __LINE__);
         continue;
      }
      tcpServerInfoObject->serverPort = atoi(res);

      tcpServerInfoObject->state      = DISCONNECTED;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumTcpServer+1, "TcpServer", "MaxLink", res, &sizeRes)) {
         printf("error: can't get LocalPort in %d lines!\n", __LINE__);
         continue;
      }
      maxLink = atoi(res);
      
      //fill gLinkMap
      gLinkMap = realloc(gLinkMap, (getSizeOfGLinkMap() + 4*sizeof(int) + maxLink*sizeof(int)));
      int x = getSizeOfGLinkMap();
      *((char**)MpLogicName(x))                   = logicName;
      *((int*)MtypeOfMap(x))                      = TCPSERVER;
      *((TcpServerInfoObject**)MpMapLinkInfo(x))  = tcpServerInfoObject;
      *((int*)MsumOfFd(x))                        = maxLink;  //sum of tcp client is 1
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

int connectTcpServer(int baseOfMap)
{
   if(0 > (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort) {
      DEBUG("serverPort is invalid:<%d>", (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort);
      return COMM_INVALID_PORT;
   }

   struct sockaddr_in clientAddr;
   struct sockaddr_in serverAddr;
   if((CONNECTED == (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->state) 
         && (fdInitValue != *(int*)MbasePoolOfFd(baseOfMap))) {
      goto accept;
   }

   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(0 > sockfd) {
      return COMM_CREATE_FD_ERROR;
   }

   serverAddr.sin_family       = AF_INET;
   serverAddr.sin_port         = htons((*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort);
   serverAddr.sin_addr.s_addr  = htons(INADDR_ANY);
   bzero(&(serverAddr.sin_zero), 8);

   if(0 > bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
      return COMM_BIND_FAILED;
   }

   if(0 > listen(sockfd, *(int*)MsumOfFd(baseOfMap))) {
      return COMM_LISTEN_FAILED;
   }

   setSocketNonBlock(sockfd);

   *((int*)MbasePoolOfFd(baseOfMap) + 0) = sockfd;
   (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->state = CONNECTED;

accept:
   while(1) {
      int length = sizeof(clientAddr);
      int newFd = accept(*(int*)MbasePoolOfFd(baseOfMap), (struct sockaddr*)&clientAddr, &length);
      if(0 > newFd) {
         DEBUG("accept failed:<%d>", newFd);
         break;
      }

      //fill into a space in fdPool
      int i;
      for(i=1; i<(*(int*)MsumOfFd(baseOfMap) - 1); i++) {
         if(-1 == *((int*)MbasePoolOfFd(baseOfMap) + i)) {
            *((int*)MbasePoolOfFd(baseOfMap) + i) = newFd;
            DEBUG("newFd is fill in gLinkMap");
            break;
         }
      }
      if(i == *(int*)MsumOfFd(baseOfMap)) {
         DEBUG("can't find space to fill newFd, maybe there's too many links in serverPort:<%d>", (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort);
         break;
      }
   }
   return COMM_SUCCESS;
}
#endif

int commInit(const char* configFilePath)
{
   const char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;
   gLinkMap = malloc(sizeof(int));
   *(int*)MsumOfMap = 0;

   /* load config info */
#ifdef TCP_CLIENT_MODE
   initTcpClientInfo(_configFilePath);
#endif

#ifdef TCP_SERVER_MODE
   initTcpServerInfo(_configFilePath);
#endif
#ifdef UDP_MODE
   //do something
#endif

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

int commGetAliveLinks(const char* logicName, int* sumFd, int** pFd)
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

int commConnect(const char* logicName)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      if((logicName) && (0 != strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
         _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
         continue;
      }

      //jump table of connect functions
      int (*connectFunc[])(int baseOfMap) = {
#ifdef TCP_CLIENT_MODE
         connectTcpClient,
#endif
#ifdef TCP_SERVER_MODE
         connectTcpServer,
#endif
#ifdef UDP_MODE
         connectUdp,
#endif
      };
      if(COMM_INVALID_MAPTYPE == checkMapType(*(int*)MtypeOfMap(_curMapStart))) {
         DEBUG("unknow map type:<%d>", *(int*)MtypeOfMap(_curMapStart));
      }
      else {
         connectFunc[*(int*)MtypeOfMap(_curMapStart)](_curMapStart);
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return COMM_SUCCESS;
}
   
int commSelect(const char* logicName)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   int _maxSockFd = 0;
   fd_set fdSet;
   FD_ZERO(&fdSet);
   while(_sumOfMap--) {
      if((logicName) && (0 != strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
         _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
         continue;
      }
      
      int i=0, _fd=0;
      for(i=0; i<*(int*)MsumOfFd(_curMapStart); i++) {
         _fd = *((int*)MbasePoolOfFd(_curMapStart) + i);
         if(fdInitValue != _fd) {
            DEBUG("LISTEN: <%d>", _fd);
            FD_SET(_fd, &fdSet);
            _maxSockFd = _maxSockFd>_fd ? _maxSockFd : _fd;
         }
      }

      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return select(_maxSockFd+1, &fdSet, NULL, NULL, &timeOutOfSelect);
}

