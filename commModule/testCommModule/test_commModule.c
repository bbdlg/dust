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

#include "../commModule.h"
#include "../../lgModule/lgModule.h"
#include "../../toolsModule/toolsModule.h"
#include "../../unity/src/unity.h"

const char* appCompileTime;
const char* appCompileDate;
const char* appVersion;
const char* commCompileTime;
const char* commCompileDate;
const char* commVersion;
const char* logCompileTime;
const char* logCompileDate;
const char* logVersion;

void setUp(void)
{
   if(gLinkMap) {
      free(gLinkMap);
   }
   gLinkMap = NULL;
}

void tearDown(void)
{
   HIDE_STDOUT
   destorySockFd(NULL);
   RESUME_STDOUT
   system("killall nc >/dev/null 2>&1");
   system("rm -rf ./commModule.conf");
}

void test_tcp_client_init(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+20, getSizeOfGLinkMap());
}

void test_tcp_client_connect(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 192.168.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+20, getSizeOfGLinkMap());

   //system("nc -l 1111 &");
   HIDE_STDOUT
   commConnect("tcpClient1");
   RESUME_STDOUT

   char res[100] = {0};
   int resSize = 100;
   //getResultFromSystemCall("netstat -an | grep 8888 | awk '{print $6}'", res, &resSize);
   //TEST_ASSERT_EQUAL_STRING("LISTEN", res);

   /*
   int* pFd = NULL;
   int _sumOfFd = 0;
   if(COMM_INVALID_LOGICNAME == commGetAliveLinks("tcpClient1", &_sumOfFd, &pFd)) {
      TEST_FAIL_MESSAGE("can't find logicName");
   }
   if(-1 == *pFd) {
      TEST_FAIL_MESSAGE("you must type 'nc -l 1111' in a new terminal to let this test instance passed");
   }
   printf("now, look at the new terminal\n");

   char* sendbuf = "must input \"1234\" and press \"ctrl+D\" in server terminal to make this test pass!\n";
   commSend(*pFd, sendbuf, strlen(sendbuf));
   char recvbuf[100] = {0};
   while(1) {
      memset(recvbuf, 0, 100);
      int recvlen = 100;
      commRecv(*pFd, recvbuf, &recvlen);
      if((recvlen) && (recvlen != -1)) {
         char* sendbuf2 = "haha, now test will pass if you type charectors on my way~~\n";
         commSend(*pFd, sendbuf2, strlen(sendbuf2));
         break;
      }
   }
   TEST_ASSERT_EQUAL_STRING("1234", recvbuf);

   //system("rm -rf commModule.conf");
   */
}

void test_tcp_server_init(void)
{
   char* content = " echo \"\
[TcpServer] \n\
LogicName   = tcpServer1 \n\
ServerPort  = 8888 \n\
MaxLink     = 10 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+16+40, getSizeOfGLinkMap());
}

void test_tcp_server_connect(void)
{
   char* content = " echo \"\
[TcpServer] \n\
LogicName   = tcpServer1 \n\
ServerPort  = 8888 \n\
MaxLink     = 10 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+16+40, getSizeOfGLinkMap());

   HIDE_STDOUT
   commConnect("tcpServer1");
   RESUME_STDOUT

   char res[100] = {0};
   int resSize = 100;
   getResultFromSystemCall("netstat -an | grep 8888 | awk '{print $6}'", res, &resSize);
   TEST_ASSERT_EQUAL_STRING("LISTEN", res);

   system("nc 127.0.0.1 8888 &");
   system("nc 127.0.0.1 8888 &");
   system("nc 127.0.0.1 8888 &");
   //sleep(1);

   getResultFromSystemCall("netstat -an | grep 8888 | wc -l", res, &resSize);
   TEST_ASSERT_EQUAL_STRING("7", res);
   //printGLinkMap(NULL);

   /*
   while(1) {
      commConnect("tcpServer1");
      printGLinkMap("tcpServer1");
      sleep(1);
   }
   */
}

void test_tcp_server_select(void)
{
   char* content = " echo \"\
[TcpServer] \n\
LogicName   = tcpServer1 \n\
ServerPort  = 8881 \n\
MaxLink     = 3 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+16+4*3, getSizeOfGLinkMap());

   HIDE_STDOUT
   commConnect("tcpServer1");
   RESUME_STDOUT

   char res[100] = {0};
   int resSize = 100;
   getResultFromSystemCall("netstat -an | grep 8881 | awk '{print $6}'", res, &resSize);
   TEST_ASSERT_EQUAL_STRING("LISTEN", res);

   //DEBUG("-------------------------");
   //while(1) {
   //   commProcess();
   //}
}

void showFunc(int argc, char* argv[])
{
   if(strcmp("show", argv[0])) {
      DEBUG("no such command:<%s>", argv[0]);
      return;
   }
   if(argv[1] && strcmp("map", argv[1])) {
      DEBUG("no such command:<%s>", argv[1]);
      return;
   }
   printGLinkMap(NULL);
   term("you tell me to show map");
}

void func0(char* buf, int* len)
{
   welcomeLp(buf, len);
}

void func1(const char* logicName, const int fd, const char* recvbuf, const int recvlen)
{
   DEBUG(">>> <%d>bytes From logicName<%s> fd<%d>: <%s> <<<", recvlen, logicName, fd, recvbuf);
   char sendbuf[1024] = {0};
   int sendlen = 0;
   procLgModule(recvbuf, recvlen, sendbuf, &sendlen);
   commSend(fd, sendbuf, &sendlen);
   DEBUG("buf:<%s>\n", sendbuf);
}

void test_commProcess(void)
{
   TEST_IGNORE_MESSAGE("you must comment this row to finish this test instance~");
   addCmdFunction(showFunc, "show");
   char* content = " echo \"\
[TcpServer] \n\
LogicName   = tcpServer1 \n\
ServerPort  = 8881 \n\
MaxLink     = 10 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   system("rm -rf commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+16+40, getSizeOfGLinkMap());

   commConnect("tcpServer1");

   commSetFunc("tcpServer1", &func0, &func1);

   printf("press ctrl+C to end this test\n");
   while(1) {
      commProcess();
   }

}

