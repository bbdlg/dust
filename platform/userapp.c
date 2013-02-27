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

#include "userapp.h"

/* Application Version Infomation */
const char* appVersion     = "1.0.0";
const char* appCompileDate = __DATE__;
const char* appCompileTime = __TIME__;


void procUdpMsg(const char* logicName, const int fd, const char* recvbuf, const int recvlen, void* from)
{
   char sendbuf[1024] = {0};
   sprintf(sendbuf, "haha, i recv <%d> bytes from u~", recvlen);
   int sendlen = strlen(sendbuf);

   printf("recv: [%s]\n", recvbuf);
   int len = sizeof(struct sockaddr);
   commSend(fd, sendbuf, &sendlen, from);
}

void sendMsgToClient(char* sendbuf, int* sendlen)
{
   memcpy(sendbuf, "1233211234567", 13);
   *sendlen = 13;
}


/*
 * Initialization Function
 * Call the following functions in this function:
 * register config file analyze function, can be called repeatedly.
 *    extern int registerCommFunc(ConfAnalyzeFunc* func);
 * register data process callback function, can be called repeatedly.
 *    extern int registerCommFunc(const char* runPara, const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);
 * register outside order response function, can be called repeatedly.
 *    extern int addCmdFunction(CmdFunction function, const char* name);
 */
void initAll(void)
{
   int ret = commSetFunc("Udp-2", NULL, &procUdpMsg);
   ret = commSetFunc("tcpClient1", &sendMsgToClient, &procUdpMsg);
   return;
}

/* Periodic Callback Function */
void checkEvent(const struct timeval curTimeval)
{
   char* data = "hallo world!\n";
   char recvdata[1024] = {0};
   int datalen = strlen(data);
   int sum;
   int* pFd = NULL;
   commGetAliveLinks("Udp-2", &sum, &pFd);
   if(pFd && (*pFd > 0)) {
      commSend(*pFd, data, &datalen, NULL);
      DEBUGINFO("send <%d> bytes~", datalen);
   }
   sleep(2);
   
   return;
}

