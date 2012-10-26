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
   char* content = " echo \"[TcpClient] \nLogicName   = tcpClient1 \nDestIp      = 192.168.0.1 \nDestPort    = 1111 \nLocalPort   = 8888 \n \n[TcpClient] \nLogicName   = tcpClient2 \nDestIp      = 192.168.0.2 \nDestPort    = 2222 \nLocalPort   = 9999 \n\n[TcpClient] \nLogicName   = tcpClient3 \nDestIp      = 192.168.0.3 \nDestPort    = 3333 \nLocalPort   = 1234 \n\" > commModule.conf";
   system(content);
   initComm("./commModule.conf");
   TEST_ASSERT_EQUAL_INT(4+20*3, getSizeOfGLinkMap());
}
