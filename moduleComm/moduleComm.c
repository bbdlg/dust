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

#include "moduleComm.h"
#include <execinfo.h>

const char* commVersion = "1.0.0";
const char* commCompileDate = __DATE__;
const char* commCompileTime = __TIME__;

void* gLinkMap = NULL;
CommFlag gCommFlag;
char gRecvBuf[MAX_LEN_BUF];
struct timeval timeOutOfSelect = {0, 100000}; //default timeout: 100ms
const char* defaultConfigFilePath = "./moduleComm.conf";

char* strSortOfCommunication[] = { //see enum SortOfCommunication
#ifdef TCP_CLIENT_MODE
   "TCPCLIENT",
#endif
#ifdef TCP_SERVER_MODE
   "TCPSERVER",
#endif
#ifdef UDP_MODE
   "UDPSERVER",
   "UDPCLIENT",
   "UDP_MULTICAST_SERVER",
   "UDP_MULTICAST_CLIENT",
#endif
   //SERIAL,
   //CAN,
   "UNKNOWN_TYPE"
};

const char* commErrInfo[commMAXERRNO] = {
   "success",
   "invalid fd",
   "invalid logic name",
   "invalid map type",
   "invalid port",
   "create socket file descripter failed",
   "bind fd failed",
   "listen fd failed",
   "connect fd failed",
   "set socket option failed",
   "unknown udp mode",
   "connection has disconnected",
   "please try again",
};

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
         cur += 4*(4 + *(int*)MsumOfFd(cur));
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

void destorySockFd(const char* logicName)
{
   int sumOfMap = *(int*)gLinkMap;
   int i=0;
   int cur=4;
   for(i=0; i<sumOfMap; i++) {
      if(logicName && strcmp(logicName, *(char**)MpLogicName(cur))) {
         cur += 4*(4 + *(int*)MsumOfFd(cur));
         continue;
      }
      int j=0;
      for(j=0; j<*(int*)MsumOfFd(cur); j++) {
         if(fdInitValue != *((int*)MbasePoolOfFd(cur) + j)) {
            close(*((int*)MbasePoolOfFd(cur) + j));
            DEBUGINFO("DESTORY %d", *((int*)MbasePoolOfFd(cur) + j));
         }
      }
      cur += 4*(4 + *(int*)MsumOfFd(cur));
   }
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
      memset(tcpClientInfoObject, 0, sizeof(TcpClientInfoObject));

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

      tcpClientInfoObject->state          = DISCONNECTED;
      tcpClientInfoObject->registerFunc   = NULL;
      tcpClientInfoObject->dataProcFunc   = NULL;
      
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
   bzero(&serverAddr, sizeof(struct sockaddr_in));
   serverAddr.sin_family       = AF_INET;
   serverAddr.sin_port         = htons(tcpClientInfoObject->destPort);
   serverAddr.sin_addr.s_addr  = inet_addr(tcpClientInfoObject->destIp);
   //bzero(serverAddr.sin_zero, sizeof(serverAddr.sin_zero));

   setSocketNonBlock(sockfd);

   /* if socket is block, connect() maybe return EINPROGRESS, it means "Operation now in progress". */
   errno = 0;
   int ret = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
#ifdef LOG
   log(LOG_WARNING, "<%s> logicName<%s> dest<%s:%d> fd<%d>, connect<%s>", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->destIp, (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->destPort, sockfd, strerror(errno));
#else
   DEBUGINFO("<%s> logicName<%s> dest<%s:%d> fd<%d>, connect<%s>", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->destIp, (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->destPort, sockfd, strerror(errno));
#endif

   if(-1 == ret) {
      if(errno != EINPROGRESS) {
         return COMM_CONNECT_FAILED;
      }
      else if(COMM_SUCCESS != commWaitFdReady(sockfd, timeOutOfSelect)) {
         return COMM_CONNECT_FAILED;
      }
   }

   //update the state and gLinkMap
   *((int*)MbasePoolOfFd(baseOfMap)) = sockfd;
   (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->state = CONNECTED;

   if((*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->registerFunc) {
      int sendlen = MAX_LEN_BUF;
      memset(gRecvBuf, 0, MAX_LEN_BUF);
      (*(TcpClientInfoObject**)MpMapLinkInfo(baseOfMap))->registerFunc(gRecvBuf, &sendlen); //borrow gRecvBuf
      commSend(sockfd, gRecvBuf, &sendlen, NULL);
   }

   return COMM_SUCCESS;
}

/*
 * RECONNECT RULES
 * time++   times
 *      5    0~2
 *     60    3~9
 *    600    10+
 */
int reconnectTcpClient(struct timeval curTimeval)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      if(TCPCLIENT != *(int*)MtypeOfMap(_curMapStart)) {
         continue;
      }
      if(NULL == (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc) {
         continue;
      }
      if((CONNECTED == (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->state)
            && (fdInitValue != *(int*)MbasePoolOfFd(_curMapStart))) {
         continue;
      }
      
      int* ptv    = &((*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->timeNextReconnect);
      int* ptimes = &((*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->timesReconnect);

      if(curTimeval.tv_sec < *ptv) {
         continue;
      }
      if(COMM_SUCCESS == connectTcpClient(_curMapStart)) {
         *ptv = 0;
         *ptimes = 0;
      }
      else{
         if(*ptimes < 0) {
            *ptimes = 0;
         }
         if(*ptimes < 3) {
            *ptv = curTimeval.tv_sec + 5;
         }
         else if((*ptimes >= 3) && (*ptimes < 10)) {
            *ptv = curTimeval.tv_sec + 60;
         }
         else {
            *ptv = curTimeval.tv_sec + 600;
         }
         (*ptimes)++;
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

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
      memset(tcpServerInfoObject, 0, sizeof(TcpServerInfoObject));

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

      tcpServerInfoObject->state          = DISCONNECTED;
      tcpServerInfoObject->registerFunc   = NULL;
      tcpServerInfoObject->dataProcFunc   = NULL;

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
      DEBUGINFO("serverPort is invalid:<%d>", (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort);
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
#ifdef LOG
   log(LOG_WARNING, "<%s> logicName<%s> localPort<%d> fd<%d>", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort, sockfd);
#else
   DEBUGINFO("<%s> logicName<%s> localPort<%d> fd<%d>", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort, sockfd);
#endif

accept:
   while(1) {
      int length = sizeof(clientAddr);
      int newFd = accept(*((int*)MbasePoolOfFd(baseOfMap) + 0), (struct sockaddr *)&clientAddr, (unsigned int*)&length);
      if(0 > newFd) {
         //DEBUG("accept failed:<%d>", newFd);
         break;
      }

      //DEBUG("newFd is fill in gLinkMap<%d>", newFd);

      //fill into a space in fdPool
      int i;
      for(i=1; i<*(int*)MsumOfFd(baseOfMap); i++) {
         if(fdInitValue == *((int*)MbasePoolOfFd(baseOfMap) + i)) {
            *((int*)MbasePoolOfFd(baseOfMap) + i) = newFd;
            //DEBUG("newFd is fill in gLinkMap<%d>", newFd);
            break;
         }
      }
      //printGLinkMap(NULL);
      if(i == *(int*)MsumOfFd(baseOfMap)) {
#ifdef LOG
         log(LOG_WARNING, "Too many links in serverPort<%d>, logicName<%s>, max<%d> links", (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort, *(char**)MpLogicName(baseOfMap), *(int*)MsumOfFd(baseOfMap));
#else
         DEBUG("Too many links in serverPort<%d>, logicName<%s>, max<%d> links", (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort, *(char**)MpLogicName(baseOfMap), *(int*)MsumOfFd(baseOfMap));
#endif
         close(newFd);
         break;
      }

#ifdef LOG
         log(LOG_INFO, "new fd<%d> connect to port<%d>,logicName<%s>", newFd, (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->serverPort, *(char**)MpLogicName(baseOfMap));
#endif

      //call registerFunc
      if((*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->registerFunc) {
         int sendlen = MAX_LEN_BUF;
         memset(gRecvBuf, 0, MAX_LEN_BUF);
         (*(TcpServerInfoObject**)MpMapLinkInfo(baseOfMap))->registerFunc(gRecvBuf, &sendlen); //borrow gRecvBuf
         commSend(newFd, gRecvBuf, &sendlen, NULL);
#ifdef LOG
         log(LOG_INFO, "call registerFunc to process fd<%d> of logicName<%s>", newFd, *(char**)MpLogicName(baseOfMap));
#endif
      }
   }
   return COMM_SUCCESS;
}
#endif

#ifdef UDP_MODE
int initUdpInfo(const char* configFilePath)
{
   int sizeRes = MAX_LEN_VALUE;
   char res[MAX_LEN_VALUE] = {0};
   int sumUdp = readValueFromConf_ext(configFilePath, 0, "Udp", "anything", res, &sizeRes);
   //printf("sumUdp is <%d>, config file path is <%s>\n", sumUdp, configFilePath);
   while(sumUdp--) {
      char* logicName = NULL;
      UdpInfoObject* udpInfoObject = (UdpInfoObject*)malloc(sizeof(UdpInfoObject));

      //read value from config file
      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumUdp+1, "Udp", "LogicName", res, &sizeRes)) {
         printf("error: can't get LogicName in %d lines!\n", __LINE__);
         continue;
      }
      logicName = (char*)malloc(sizeRes+1);
      memcpy(logicName, res, sizeRes);
      logicName[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumUdp+1, "Udp", "DestIp", res, &sizeRes)) {
         printf("error: can't get DestIp %d lines!\n", __LINE__);
         continue;
      }
      udpInfoObject->destIp = (char*)malloc(sizeRes+1);
      //DEBUG("IP p=%p,<%s>len<%d>", udpInfoObject->destIp, res, sizeRes);
      memcpy(udpInfoObject->destIp, res, sizeRes);
      udpInfoObject->destIp[sizeRes] = 0;

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumUdp+1, "Udp", "DestPort", res, &sizeRes)) {
         printf("error: can't get DestPort in %d lines!\n", __LINE__);
         continue;
      }
      udpInfoObject->destPort = atoi(res);

      sizeRes = MAX_LEN_VALUE;
      memset(res, 0, sizeRes);
      if(TOOLS_CANNOT_FIND_VALUES == readValueFromConf_ext(configFilePath, sumUdp+1, "Udp", "LocalPort", res, &sizeRes)) {
         printf("error: can't get LocalPort in %d lines!\n", __LINE__);
         continue;
      }
      udpInfoObject->localPort = atoi(res);

      udpInfoObject->state          = DISCONNECTED;
      udpInfoObject->registerFunc   = NULL;
      udpInfoObject->dataProcFunc   = NULL;
      
      //check udp mode
    /* udpserver: destIp null, destPort null, localPort not null.
    * udpclient: destIp not null, destPort not null, localPort any.
    * udp multicast server: destIp not null, destPort null, localPort not null.
    * udp multicast client: destIp not null, destPort not null, localPort null.
    * multicast destIp must be in 224.0.0.0 ~ 239.255.255.255.
    */
      int udp_mode = UNKNOWN_TYPE;
      if((inet_addr(udpInfoObject->destIp) == INADDR_NONE) && (udpInfoObject->destPort <= 0) && (udpInfoObject->localPort > 0)) {
         udp_mode = UDPSERVER;
      }
      else if((inet_addr(udpInfoObject->destIp) != INADDR_NONE) && (udpInfoObject->destPort > 0) && (udpInfoObject->localPort <= 0)) {
         udp_mode = UDPCLIENT;
      }
      else {
         printf("-- unknown udp mode\n");
      }

      //fill gLinkMap
      gLinkMap = realloc(gLinkMap, (getSizeOfGLinkMap() + 4*sizeof(int) + 1*sizeof(int)));
      int x = getSizeOfGLinkMap();
      *((char**)MpLogicName(x))                   = logicName;
      *((int*)MtypeOfMap(x))                      = udp_mode;
      *((UdpInfoObject**)MpMapLinkInfo(x))        = udpInfoObject;
      *((int*)MsumOfFd(x))                        = 1;  //sum of udp  is 1
      MinitPoolOfFd(x);
      *((int*)MsumOfMap)                          = *((int*)MsumOfMap) + 1;
   }

   return COMM_SUCCESS;
}

int connectUdp(int baseOfMap)
{
   if((CONNECTED == (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->state) 
         && (fdInitValue != *(int*)MbasePoolOfFd(baseOfMap))) {
      return COMM_SUCCESS;
   }

   int ret = 0;
   UdpInfoObject* udpInfoObject = *(UdpInfoObject**)MpMapLinkInfo(baseOfMap);
   int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(0 > sockfd) {
      return COMM_CREATE_FD_ERROR;
   }
   struct sockaddr_in serverAddr;
   bzero(&serverAddr, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   if(*(int*)MtypeOfMap(baseOfMap) == UDPCLIENT) {
      serverAddr.sin_port         = htons(udpInfoObject->destPort);
      serverAddr.sin_addr.s_addr  = inet_addr(udpInfoObject->destIp);
   }
   else if(*(int*)MtypeOfMap(baseOfMap) == UDPSERVER) {
      serverAddr.sin_port         = htons(udpInfoObject->localPort);
      serverAddr.sin_addr.s_addr  = htonl(INADDR_ANY);
      //DEBUGINFO("PORT: %d, IP: %d", udpInfoObject->localPort, INADDR_ANY);
   }

   setSocketNonBlock(sockfd);

   
   if(*(int*)MtypeOfMap(baseOfMap) == UDPCLIENT) {
      ret = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
   }
   else if(*(int*)MtypeOfMap(baseOfMap) == UDPSERVER) {
      ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
   }
   else {
      DEBUGINFO("unknown type:<%d>", *(int*)MtypeOfMap(baseOfMap));
      return COMM_CONNECT_FAILED;
   }
   if(-1 == ret) {
      return COMM_CONNECT_FAILED;
   }

   *((int*)MbasePoolOfFd(baseOfMap)) = sockfd;
   (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->state = CONNECTED;
#ifdef LOG
   log(LOG_WARNING, "<%s> logicName<%s> destIp<%s> destPort<%d> localPort<%d> fd: %d", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->destIp, (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->destPort, (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->localPort, sockfd);
#else
   DEBUGINFO("<%s> logicName<%s> destIp<%s> destPort<%d> localPort<%d> fd: %d", strSortOfCommunication[*(int*)MtypeOfMap(baseOfMap)], *(char**)MpLogicName(baseOfMap), (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->destIp, (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->destPort, (*(UdpInfoObject**)MpMapLinkInfo(baseOfMap))->localPort, sockfd);
#endif

   return COMM_SUCCESS;
}
#endif

void commFreeAllFds(void)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      int sumFd = *(int*)MsumOfFd(_curMapStart);
      int* pFd  = (int*)MbasePoolOfFd(_curMapStart);
      int i=0;
      for(i=0; i<sumFd; i++) {
         if(*(pFd+i) < 0) continue;
         shutdown(*(pFd+i), 2);
         DEBUGINFO("close fd<%d>", *(pFd+i));
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }
}

int commWaitFdReady(const int fd, const struct timeval *timeout)
{
   fd_set wfd;
   struct timeval to;
   struct timeval *toptr = NULL;

   if(0 > fd) {
      return COMM_INVALID_FD;
   }
   if(timeout) {
      to = *timeout;
      toptr = &to;
   }

   FD_ZERO(&wfd);
   FD_SET(fd, &wfd);
   if(-1 == select(fd+1, NULL, &wfd, NULL, toptr)) {
      goto failed;
   }
   if(!FD_ISSET(fd, &wfd)) {
      errno = ETIMEDOUT;
      close(fd);
      goto failed;
   }

   int err = 0;
   int errlen = sizeof(err);
   if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
      close(fd);
      goto failed;
   }

   if (err) {
      errno = err;
      close(fd);
      goto failed;
   }
   
   return COMM_SUCCESS;

failed:
#ifdef LOG
   log(LOG_WARNING, "wait fd ready failed: <%s>", strerror(errno));
#else
   DEBUGINFO("wait fd ready failed: <%s>", strerror(errno));
#endif
   return COMM_CONNECT_FAILED;
}

int commInit(const char* configFilePath)
{
   int ret;
   const char* _configFilePath = (NULL==configFilePath) ? defaultConfigFilePath : configFilePath;
   gLinkMap = malloc(sizeof(int));
   *(int*)MsumOfMap = 0;

   /* load config info */
#ifdef TCP_CLIENT_MODE
   ret = initTcpClientInfo(_configFilePath);
   if(ret) {
      return ret;
   }
#endif

#ifdef TCP_SERVER_MODE
   ret = initTcpServerInfo(_configFilePath);
   if(ret) {
      return ret;
   }
#endif
#ifdef UDP_MODE
   ret = initUdpInfo(_configFilePath);
   if(ret) {
      return ret;
   }
#endif

   /* init global varible */
   memset(&gCommFlag, ON, sizeof(CommFlag));

   return COMM_SUCCESS;
}

int commRecv(int* fd, char* recvBuf, int* recvLen, void* from)
{
   if(0 >= *recvLen) {
      //warning
   }

   //DEBUGINFO("fd:%d, pbuf:%p, len:%d, from:%p", *fd, recvBuf, *recvLen, from);

   errno = 0;
   if(NULL == from) {
      *recvLen = recv(*fd, recvBuf, *recvLen, MSG_DONTWAIT);
   }
   else {
#ifdef UDP_MODE
      int len = sizeof(struct sockaddr);
      *recvLen = recvfrom(*fd, recvBuf, *recvLen, MSG_DONTWAIT, (struct sockaddr*)from, &len);
#endif
   }

   if(0 < *recvLen) {
#ifdef LOG
      log(LOG_INFO, "recv %d bytes via fd<%d>", *recvLen, *fd);
#endif
   }
   else if(0 == *recvLen) {
      if(errno != EINTR) {
         return COMM_DISCONNECTED;
      }
   }
   else { //(0 > *recvLen)
      if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
         return COMM_TRY_AGAIN;
      }
      else{
         return COMM_DISCONNECTED;
      }
   }

   //trace
   if(gCommFlag.tcpClientTrace || gCommFlag.tcpServerTrace || gCommFlag.udpClientTrace || gCommFlag.udpServerTrace) {
      int pos = commGetMposByFd(*fd);
      switch(*(int*)MtypeOfMap(pos)) {
#ifdef TCP_CLIENT_MODE
         case TCPCLIENT:
            if(ON == gCommFlag.tcpClientTrace) {
               (*(TcpClientInfoObject**)MpMapLinkInfo(pos))->recordObj.recvPacket++;
               (*(TcpClientInfoObject**)MpMapLinkInfo(pos))->recordObj.recvByte += *recvLen;
            }
            if(ON == gCommFlag.tcpClientPrint) {
               DEBUGINFO("recv len is %d", *recvLen);
               hexdump(recvBuf, *recvLen, "get msg :");
            }
            break;
#endif
#ifdef TCP_SERVER_MODE
         case TCPSERVER:
            if(ON == gCommFlag.tcpServerTrace) {
               (*(TcpServerInfoObject**)MpMapLinkInfo(pos))->recordObj.recvPacket++;
               (*(TcpServerInfoObject**)MpMapLinkInfo(pos))->recordObj.recvByte += *recvLen;
            }
            if(ON == gCommFlag.tcpServerPrint) {
               DEBUGINFO("recv len is %d", *recvLen);
               hexdump(recvBuf, *recvLen, "get msg :");
            }
            break;
#endif
      }

   }

   return COMM_SUCCESS;
}

int commSend(int fd, const char* sendBuf, int* sendLen, void* to)
{
   if(0 == sendLen) {
      //warning
   }

   if(NULL == to) {
      *sendLen = send(fd, sendBuf, *sendLen, MSG_DONTWAIT);
   }
   else {
      *sendLen = sendto(fd, sendBuf, *sendLen, MSG_DONTWAIT, (struct sockaddr*)to, sizeof(struct sockaddr));
   }

#ifdef LOG
   log(LOG_INFO, "send %d bytes via fd<%d>", *sendLen, fd);
#endif

   //trace
   if(gCommFlag.tcpClientTrace || gCommFlag.tcpServerTrace || gCommFlag.udpClientTrace || gCommFlag.udpServerTrace) {
      int pos = commGetMposByFd(fd);
      switch(*(int*)MtypeOfMap(pos)) {
#ifdef TCP_CLIENT_MODE
         case TCPCLIENT:
            if(ON == gCommFlag.tcpClientTrace) {
               (*(TcpClientInfoObject**)MpMapLinkInfo(pos))->recordObj.sendPacket++;
               (*(TcpClientInfoObject**)MpMapLinkInfo(pos))->recordObj.sendByte += *sendLen;
            }
            if(ON == gCommFlag.tcpClientPrint) {
               DEBUGINFO("send len is %d", *sendLen);
               hexdump(sendBuf, *sendLen, "sent msg :");
            }
            break;
#endif
#ifdef TCP_SERVER_MODE
         case TCPSERVER:
            if(ON == gCommFlag.tcpServerTrace) {
               (*(TcpServerInfoObject**)MpMapLinkInfo(pos))->recordObj.sendPacket++;
               (*(TcpServerInfoObject**)MpMapLinkInfo(pos))->recordObj.sendByte += *sendLen;
            }
            if(ON == gCommFlag.tcpServerPrint) {
               DEBUGINFO("send len is %d", *sendLen);
               hexdump(sendBuf, *sendLen, "sent msg :");
            }
            break;
#endif
#ifdef UDP_MODE
         case UDPSERVER:
            if(ON == gCommFlag.udpServerTrace) {
               (*(UdpInfoObject**)MpMapLinkInfo(pos))->recordObj.sendPacket++;
               (*(UdpInfoObject**)MpMapLinkInfo(pos))->recordObj.sendByte += *sendLen;
            }
            if(ON == gCommFlag.udpServerPrint) {
               DEBUGINFO("send len is %d", *sendLen);
               hexdump(sendBuf, *sendLen, "sent msg :");
            }
            break;
         case UDPCLIENT:
            if(ON == gCommFlag.udpClientTrace) {
               (*(UdpInfoObject**)MpMapLinkInfo(pos))->recordObj.sendPacket++;
               (*(UdpInfoObject**)MpMapLinkInfo(pos))->recordObj.sendByte += *sendLen;
            }
            if(ON == gCommFlag.udpClientPrint) {
               DEBUGINFO("send len is %d", *sendLen);
               hexdump(sendBuf, *sendLen, "sent msg :");
            }
            break;
         case UDP_MULTICAST_SERVER:
         case UDP_MULTICAST_CLIENT:
            DEBUGINFO("multicast udp server or client!");
            break;
#endif
      }

   }

   return COMM_SUCCESS;
}

int commGetAliveLinks(const char* logicName, int* sumFd, int** pFd)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      int flag = 0;
      if(0) {}
#ifdef TCP_CLIENT_MODE
      else if((TCPCLIENT == *(int*)MtypeOfMap(_curMapStart)) && (CONNECTED == (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->state) && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
         flag = 1;
      }
#endif
#ifdef TCP_SERVER_MODE
      else if((TCPSERVER == *(int*)MtypeOfMap(_curMapStart)) && (CONNECTED == (*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->state) && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
         flag = 2;
      }
#endif
#ifdef UDP_MODE
      else if((   (UDPCLIENT            == *(int*)MtypeOfMap(_curMapStart)) 
               || (UDPSERVER            == *(int*)MtypeOfMap(_curMapStart)) 
               || (UDP_MULTICAST_SERVER == *(int*)MtypeOfMap(_curMapStart)) 
               || (UDP_MULTICAST_CLIENT == *(int*)MtypeOfMap(_curMapStart)))
          && (CONNECTED == (*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->state) 
          && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
         flag = 3;
      }
#endif

      if(flag) {
         *sumFd = *(int*)MsumOfFd(_curMapStart);
         *pFd = (int*)MbasePoolOfFd(_curMapStart);
#ifdef TCP_SERVER_MODE
         if(TCPSERVER == *(int*)MtypeOfMap(_curMapStart)) {
            (*sumFd)--;
            (*pFd)++;
         }
#endif
         return COMM_SUCCESS;
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return COMM_INVALID_LOGICNAME;
}

int commGetMposByLogicName(const char* logicName)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      if(0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName)) {
         return _curMapStart;
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return 0;
}

int commGetMposByFd(const int fd)
{
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      int i=0;
      for(i=0; i<*(int*)MsumOfFd(_curMapStart); i++) {
         if(fd == *((int*)MbasePoolOfFd(_curMapStart)+i)) {
            return _curMapStart;
         }
      }
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   //cant't find Map Position.
   return 0;
}

int commConnect(const char* logicName)
{
   int _sumOfMap = *(int*)gLinkMap;
   int ret = 0;
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
         connectUdp,
         connectUdp,
         connectUdp,
#endif
      };
      if(COMM_INVALID_MAPTYPE == checkMapType(*(int*)MtypeOfMap(_curMapStart))) {
#ifdef LOG
         log(LOG_WARNING, "unknow map type:<%d>", *(int*)MtypeOfMap(_curMapStart));
#else
         DEBUGINFO("unknow map type:<%d>", *(int*)MtypeOfMap(_curMapStart));
#endif
      }
      else {
         ret = connectFunc[*(int*)MtypeOfMap(_curMapStart)](_curMapStart);
         if(ret) {
#ifdef LOG
            log(LOG_ERROR, "connect logicName<%s> failed! reason<%s>", *(char**)MpLogicName(_curMapStart), moduleErrInfo(comm, ret));
#endif
         }
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
#ifdef TCP_CLIENT_MODE
            if((TCPCLIENT == *(int*)MtypeOfMap(_curMapStart)) 
                  && (CONNECTED != (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->state)) {
               continue;
            }
#endif
            //DEBUGINFO("select: <%d>", _fd);
            FD_SET(_fd, &fdSet);
            _maxSockFd = _maxSockFd>_fd ? _maxSockFd : _fd;
         }
      }

      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   return select(_maxSockFd+1, &fdSet, NULL, NULL, &timeOutOfSelect);
}

int commSetFunc(const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc)
{
   int sumOfMap = *(int*)gLinkMap;
   int i=0;
   int cur=4;
   for(i=0; i<sumOfMap; i++) {
      if(logicName && strcmp(logicName, *(char**)MpLogicName(cur))) {
         cur += 4*(4 + *(int*)MsumOfFd(cur));
         continue;
      }

      if(0) {
         printf("you are never get here~~");
      }
#ifdef TCP_SERVER_MODE
      else if(TCPSERVER == *(int*)MtypeOfMap(cur)) {
         (*(TcpServerInfoObject**)MpMapLinkInfo(cur))->registerFunc = registerFunc;
         (*(TcpServerInfoObject**)MpMapLinkInfo(cur))->dataProcFunc = dataProcFunc;
#ifdef LOG
         log(LOG_INFO, "register function of logicName<%s>, type<TCP_SERVER>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#else
         DEBUGINFO("register function of logicName<%s>, type<TCP_SERVER>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#endif
      }
#endif
#ifdef TCP_CLIENT_MODE
      else if(TCPCLIENT == *(int*)MtypeOfMap(cur)) {
         (*(TcpClientInfoObject**)MpMapLinkInfo(cur))->registerFunc = registerFunc;
         (*(TcpClientInfoObject**)MpMapLinkInfo(cur))->dataProcFunc = dataProcFunc;
#ifdef LOG
         log(LOG_INFO, "register function of logicName<%s>, type<TCP_CLIENT>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#else
         DEBUGINFO("register function of logicName<%s>, type<TCP_CLIENT>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#endif
      }
#endif
#ifdef UDP_MODE
      else if( (UDPSERVER == *(int*)MtypeOfMap(cur)) 
            || (UDPCLIENT == *(int*)MtypeOfMap(cur)) 
            || (UDP_MULTICAST_SERVER == *(int*)MtypeOfMap(cur)) 
            || (UDP_MULTICAST_CLIENT == *(int*)MtypeOfMap(cur))) { 
         (*(UdpInfoObject**)MpMapLinkInfo(cur))->registerFunc = registerFunc;
         (*(UdpInfoObject**)MpMapLinkInfo(cur))->dataProcFunc = dataProcFunc;
#ifdef LOG
         log(LOG_INFO, "register function of logicName<%s>, type<UDP>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#else
         DEBUGINFO("register function of logicName<%s>, type<UDP>, addr<%p><%p>", logicName, registerFunc, dataProcFunc);
#endif
      }
#endif
      return COMM_SUCCESS;
   }

   return COMM_INVALID_LOGICNAME;
}

int commProcess(struct timeval curTimeval)
{
   int ret = 0;
   while((ret = commSelect(NULL))) {
      DEBUGINFO("select return %d", ret);
      //recv all fd
      int _sumOfMap = *(int*)gLinkMap;
      int _curMapStart = sizeof(int);
      while(_sumOfMap--) {
         int i = 0;
         for(i=0; i<*(int*)MsumOfFd(_curMapStart); i++) {
            if(fdInitValue == *((int*)MbasePoolOfFd(_curMapStart)+i)) {
               continue;
            }
            int recvlen = MAX_LEN_BUF;
            memset(gRecvBuf, 0, recvlen);

#ifdef TCP_SERVER_MODE
            if((TCPSERVER == *(int*)MtypeOfMap(_curMapStart)) 
                  && ((*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc)) 
            {
               if(0 == i) { //first fd is listen fd of tcp server.
                  continue;
               }
               if(COMM_DISCONNECTED == commRecv(((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, &recvlen, NULL)) {
                  DEBUGINFO("close server fd<%d>", *((int*)MbasePoolOfFd(_curMapStart) + i));
                  close(*((int*)MbasePoolOfFd(_curMapStart) + i));
                  *((int*)MbasePoolOfFd(_curMapStart) + i) = fdInitValue;
               }
               if(recvlen <= 0) {
                  continue;
               }
              (*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc( *(char**)MpLogicName(_curMapStart), *((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, recvlen, NULL);
            }
#endif
#ifdef TCP_CLIENT_MODE
            if((TCPCLIENT == *(int*)MtypeOfMap(_curMapStart)) 
                  && ((*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc)) 
            {
               if(COMM_DISCONNECTED == commRecv(((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, &recvlen, NULL)) {
                  close(*((int*)MbasePoolOfFd(_curMapStart) + i));
                  *((int*)MbasePoolOfFd(_curMapStart) + i) = fdInitValue;
                  (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->state = DISCONNECTED;
               }
               if(recvlen <= 0) {
                  continue;
               }
               (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc( *(char**)MpLogicName(_curMapStart), *((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, recvlen, NULL);
            }
#endif
#ifdef UDP_MODE
            int tmpType = *(int*)MtypeOfMap(_curMapStart); 
            if(((UDPSERVER == tmpType) || (UDPCLIENT == tmpType) || (UDP_MULTICAST_SERVER == tmpType) || (UDP_MULTICAST_CLIENT == tmpType))
              && ((*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc)) 
            {
               struct sockaddr sAddr;
               bzero(&sAddr, sizeof(struct sockaddr));
               commRecv(((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, &recvlen, &sAddr);
               if(recvlen <= 0) {
                  continue;
               }
               (*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->dataProcFunc( *(char**)MpLogicName(_curMapStart), *((int*)MbasePoolOfFd(_curMapStart)+i), gRecvBuf, recvlen, &sAddr);
           }
#endif
         }
         _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
      }
      
      //try to accept
      commConnect(NULL);
   }

#ifdef TCP_CLIENT_MODE
   reconnectTcpClient(curTimeval);
#endif

   return COMM_SUCCESS;
}

void lgCmdFuncTrace(int argc, char* argv[])
{
   if(1 == argc) {
      term("Usage: trace [all|tcp|udp|logicName]\n");
      return;
   }

   char flag = -1;
   if(0 == strcmp(argv[1], "all"))        flag = 1;
   if(0 == strcmp(argv[1], "tcp"))        flag = 2;
   if(0 == strcmp(argv[1], "udp"))        flag = 3;
   const char* logicName = (-1==flag) ? argv[1] : argv[2];
   int has_record= 0;
   int _sumOfMap = *(int*)gLinkMap;
   int _curMapStart = sizeof(int);
   while(_sumOfMap--) {
      int type = *(int*)MtypeOfMap(_curMapStart);
      DEBUGINFO("type is %d", type);
      if(0) {}
#ifdef TCP_CLIENT_MODE
      else if(TCPCLIENT == type) {
         if(1 == flag) {
            RecordObj obj = (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(2 == flag) {
            RecordObj obj = (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(logicName && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
            RecordObj obj = (*(TcpClientInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
      }
#endif
#ifdef TCP_SERVER_MODE
      else if(TCPSERVER == type) {
         if(1 == flag) {
            RecordObj obj = (*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(2 == flag) {
            RecordObj obj = (*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(logicName && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
            RecordObj obj = (*(TcpServerInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
      }
#endif
#ifdef UDP_MODE
      else if((UDPSERVER == type) || (UDPCLIENT == type) || (UDP_MULTICAST_SERVER == type) || (UDP_MULTICAST_CLIENT == type)) {
         DEBUGINFO("------------ flag = %d, logic<%s>-<%s>", flag, logicName, *(char**)MpLogicName(_curMapStart));
         if(1 == flag) {
            RecordObj obj = (*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(3 == flag) {
            RecordObj obj = (*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
         else if(logicName && (0 == strcmp(*(char**)MpLogicName(_curMapStart), logicName))) {
            RecordObj obj = (*(UdpInfoObject**)MpMapLinkInfo(_curMapStart))->recordObj;
            term("logicName:<%s>, \tsend:<%d>packets,<%d>bytes, recv:<%d>packets,<%d>bytes\n", 
                  *(char**)MpLogicName(_curMapStart), obj.sendPacket, obj.sendByte, obj.recvPacket, obj.recvByte);
            has_record = 1;
         }
      }
#endif
      _curMapStart += 4*(4 + *(int*)MsumOfFd(_curMapStart));
   }

   if(0 == has_record) {
      term("Try 'trace all'");
   }

   return;
}

