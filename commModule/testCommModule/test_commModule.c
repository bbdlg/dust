#include "../commModule.h"
#include "../../unity/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_initLog_initComm(void)
{
   char* content = " echo \"\
[TcpClient] \n\
LogicName   = tcpClient1 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 1111 \n\
LocalPort   = 8888 \n \
\n\
[TcpClient] \n\
LogicName   = tcpClient2 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 2222 \n\
LocalPort   = 9999 \n\
\n\
[TcpClient] \n\
LogicName   = tcpClient3 \n\
DestIp      = 127.0.0.1 \n\
DestPort    = 3333 \n\
LocalPort   = 1234 \n\"        > commModule.conf";
   system(content);
   initComm("./commModule.conf");
   printGLinkMap();
   TEST_ASSERT_EQUAL_INT(4+20*3, getSizeOfGLinkMap());

   int* pFd = NULL;
   int _sumOfFd = 0;
   if(COMM_INVALID_LOGICNAME == getAliveLink("tcpClient1", &_sumOfFd, &pFd)) {
      TEST_FAIL_MESSAGE("can't find logicName");
   }
   if(-1 == *pFd) {
      TEST_FAIL_MESSAGE("you can in a new terminal type 'nc -l 1111' to let me go on");
   }

   char* sendbuf = "must input \"1234554321\" and press \"ctrl+D\" in server terminal to make this test pass!\n";
   commSend(*pFd, sendbuf, strlen(sendbuf));
   char recvbuf[100] = {0};
   while(1) {
      memset(recvbuf, 0, 100);
      int recvlen = 100;
      commRecv(*pFd, recvbuf, &recvlen);
      if((recvlen) && (recvlen != -1)) {
         char* sendbuf2 = "haha, now test if you type charectors on my way~~\n";
         commSend(*pFd, sendbuf2, strlen(sendbuf2));
         break;
      }
      sleep(2);
   }
   TEST_ASSERT_EQUAL_STRING("1234554321", recvbuf);

   system("rm -rf commModule.conf");
}
