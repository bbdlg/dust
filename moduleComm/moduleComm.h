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

#ifndef _COMMMDULE_H_
#define _COMMMDULE_H_
/****************************************************************************
 * name:     comm模块
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    通过在配置文件中配置参数，在程序中可自动完成相应的处理。如在配置文件中
 * 配置一个TcpServer监听1986端口，程序自动读取该配置文件，并完成该端口的监听
 * 过程。
 *    
*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "moduleTools.h"
#ifdef LOG
   #include "moduleLog.h"
   #include "moduleErr.h"
#endif

/*
 * Version and Compile Info
 * */
extern const char* commVersion;
extern const char* commCompileDate;
extern const char* commCompileTime;

/*
 * Error in this module
 * */
enum commErrNo {
   COMM_SUCCESS = 0,
   COMM_INVALID_LOGICNAME,
   COMM_INVALID_MAPTYPE,
   COMM_INVALID_PORT,
   COMM_CREATE_FD_ERROR,
   COMM_BIND_FAILED,
   COMM_LISTEN_FAILED,
   COMM_CONNECT_FAILED,
   COMM_SET_SOCKET_FAILED,
   COMM_UNKNOWN_UDP_MODE,
   commMAXERRNO
};
extern const char* commErrInfo[commMAXERRNO];

/*
 * type of logicname
 * */
enum SortOfCommunication {
#ifdef TCP_CLIENT_MODE
   TCPCLIENT,
#endif
#ifdef TCP_SERVER_MODE
   TCPSERVER,
#endif
#ifdef UDP_MODE
   UDPSERVER,
   UDPCLIENT,
   UDP_MULTICAST_SERVER,
   UDP_MULTICAST_CLIENT,
#endif
   //SERIAL,
   //CAN,
   UNKNOWN_TYPE
};
#define checkMapType(type)    (type>=UNKNOWN_TYPE ? COMM_INVALID_MAPTYPE : COMM_SUCCESS)

/*
 * type of state
 * */
enum TypeOfState {
   CONNECTED = 0,
   DISCONNECTED,
};

/*
 * map of logicName and file descriptors.
 * gLinkMap points to a piece of memory, which have such struct:
 *    type     size           meaning
 *    Mapinfo  4              sum of map  
 *    Map1     4              pointer of logicname1 string
 *             4              type of logicname1
 *             4              pointer of logicname1 link info struct
 *             4              sum of fd is MaxLink1
 *             4 * MaxLink1   
 *    MapN      ……            other map, same as Map1 
 *       
 * */
extern void* gLinkMap;

/*
 * call this function before use other functions in commModule
 * */
extern int commInit(const char* configFilePath);

/*
 * interface of recv data
 * recvBuf and recvLen are all Value-result parameter.
 * */
extern int commRecv(int *fd, char* recvBuf, int* recvLen, struct sockaddr* from);

/*
 * interface of send data
 * */
extern int commSend(int fd, const char* sendBuf, int* sendLen, struct sockaddr* to);

/*
 * get all file descriptors associated with logicName.
 * put fd in pFd, and sum of fd in sumFd.
 * */
extern int commGetAliveLinks(const char* logicName, int* sumFd, int** pFd);

/*
 * return the position of logicName in gLinkMap.
 * */
extern int commGetLogicNamePos(const char* logicName);

/*
 * if logicName is null, it will maintain all logicNames,
 * or the logicName only.
 * */
extern int commConnect(const char* logicName);

/*
 * if logicName is null, it will maintain all logicNames,
 * or the logicName only.
 * */
extern int commSelect(const char* logicName);

typedef void RegisterFunc(char* recvbuf, int* recvlen);
typedef void DataProcFunc(const char* logicName, const int fd, const char* recvbuf, const int recvlen);
extern int commSetFunc(const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);

/* 
 * before call this function, you must call commInit() and commConnect(NULL);
 * you call this function periodically.
 * */
extern int commProcess(void);

#define MAX_LEN_VALUE   100
#define MAX_LEN_BUF     4096
#define MsumOfMap                   gLinkMap
#define MbaseMap(sizeOfGLinkMap)    (MsumOfMap + sizeOfGLinkMap)
#define MpLogicName(x)              MbaseMap(x)
#define MtypeOfMap(x)               (MpLogicName(x) + sizeof(char*))
#define MpMapLinkInfo(x)            (MtypeOfMap(x) + sizeof(int))
#define MsockaddrFrom(x)            (MpMapLinkInfo(x) + sizeof(int*))
#define MsumOfFd(x)                 (MsockaddrFrom(x) + sizeof(struct sockaddr))
#define MbasePoolOfFd(x)            (MsumOfFd(x) + sizeof(int))
#define fdInitValue                 -1
#define MinitPoolOfFd(x)            memset(MbasePoolOfFd(x), fdInitValue, (*(int*)MsumOfFd(x) * sizeof(int)))
extern int getSizeOfGLinkMap(void);
extern void destorySockFd(const char* logicName);
extern void printGLinkMap(const char* logicName);

#ifdef TCP_CLIENT_MODE
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   char* destIp;
   int   destPort;
   int   localPort;
   int   state;
}TcpClientInfoObject;
#endif

#ifdef TCP_SERVER_MODE
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   int   serverPort;
   int   state;
}TcpServerInfoObject;
#endif

#ifdef UDP_MODE
/*
 * udpserver: destIp null, destPort null, localPort not null.
 * udpclient: destIp not null, destPort not null, localPort any.
 * udp multicast server: destIp not null, destPort null, localPort not null.
 * udp multicast client: destIp not null, destPort not null, localPort null.
 * multicast destIp must be in 224.0.0.0 ~ 239.255.255.255.
 * */
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   char* destIp;
   int   destPort;
   int   localPort;
   int   state;
}UdpInfoObject;
#endif

#endif

