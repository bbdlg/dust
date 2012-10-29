#include "../commModule.h"
#include "../../tools/tools.h"
#include "../../unity/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_tcp_client(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   initComm("./commModule.conf");
   //printGLinkMap();
   TEST_ASSERT_EQUAL_INT(4+20, getSizeOfGLinkMap());

   int* pFd = NULL;
   int _sumOfFd = 0;
   if(COMM_INVALID_LOGICNAME == getAliveLink("tcpClient1", &_sumOfFd, &pFd)) {
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
}

void test_tcp_server(void)
{
   char* content = " echo \"\
[TcpServer] \n\
LogicName   = tcpClient1 \n\
ServerPort  = 8888 \n\
MaxLink     = 10 \n\"        > commModule.conf";
   system(content);
   initComm("./commModule.conf");
   //printGLinkMap();
   TEST_ASSERT_EQUAL_INT(4+16+40, getSizeOfGLinkMap());

   char res[100] = {0};
   int resSize = 100;
   getResultFromSystemCall("netstat -an | grep 8888 | awk '{print $6}'", res, &resSize);
   TEST_ASSERT_EQUAL_STRING("LISTEN", res);

   system("rm -rf commModule.conf");
}
