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
#include "toolsModule.h"
#ifdef LOG
   #include "logModule.h"
   #include "errModule.h"
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
   UDP,
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
extern int commRecv(int *fd, char* recvBuf, int* recvLen);

/*
 * interface of send data
 * */
extern int commSend(int fd, const char* sendBuf, int* sendLen);

/*
 * get all file descriptors associated with logicName.
 * put fd in pFd, and sum of fd in sumFd.
 * */
extern int commGetAliveLinks(const char* logicName, int* sumFd, int** pFd);

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
#define MsumOfFd(x)                 (MpMapLinkInfo(x) + sizeof(int*))
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

#endif

