#include "../commModule.h"
#include "../../tools/tools.h"
#include "../../unity/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
   destorySockFd(NULL);
   system("killall nc >/dev/null 2>&1");
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
   commConnect("tcpClient1");

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

   commConnect("tcpServer1");

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
ServerPort  = 8880 \n\
MaxLink     = 10 \n\"        > commModule.conf";
   system(content);
   commInit("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+16+40, getSizeOfGLinkMap());

   commConnect("tcpServer1");

   char res[100] = {0};
   int resSize = 100;
   getResultFromSystemCall("netstat -an | grep 8880 | awk '{print $6}'", res, &resSize);
   TEST_ASSERT_EQUAL_STRING("LISTEN", res);

   system("nc 127.0.0.1 8880 &");
   system("nc 127.0.0.1 8880 &");
   system("nc 127.0.0.1 8880 &");

   int ret = commSelect("tcpServer1");
   TEST_ASSERT_EQUAL_INT(1, ret);
   commConnect("tcpServer1");
   //system("netstat -an|grep 8880");

   system("rm -rf commModule.conf");
}

