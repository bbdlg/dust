#include "../lgModule.h"
#include "unity.h"

void setUp(void)
{

}

void tearDown(void)
{
   memset(poolCmdFunction, 0, sizeof(poolCmdFunction));
}

void test_initLgModule(void)
{
   TEST_ASSERT_EQUAL_INT(LG_NO_WELCOMEINFO, initLgModule(NULL));
   //TEST_ASSERT_EQUAL_INT(LG_SUCCESS, initLgModule("welcome to test lgModule"));
}

void test_welcomeLp(void)
{
   char sendbuf[1024] = {0};
   int sendlen = 0;
   welcomeLp(sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("WELCOME TO LGMODULE\nlpsay: ", sendbuf);
   
   initLgModule("HALLO, BB~");
   memset(sendbuf, 0, sizeof(sendbuf)/sizeof(sendbuf[0]));
   welcomeLp(sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("HALLO, BB~\nlpsay: ", sendbuf);
}

void exampleCmdFunction(int argc, char* argv[])
{
   int i = argc;
   while(argc--) {
      term("%s", argv[argc]);
   }
}

void test_addCmdFunction(void)
{
   int i=0;
   for(i=0; i<MAX_CMD_FUNCTION; i++) {
      char ctmp[10]={0};
      sprintf(ctmp, "cmd%d", i);
      TEST_ASSERT_EQUAL_INT(LG_SUCCESS, addCmdFunction(exampleCmdFunction, ctmp));
   }
   TEST_ASSERT_EQUAL_INT(LG_OVERMUCH_CMDFUNCTION, addCmdFunction(exampleCmdFunction, "cmdn"));
}

void test_procLgModule(void)
{
   addCmdFunction(exampleCmdFunction, "testcmd");
   const char recvbuf[] = "testcmd 1 2 3 4t";
   char sendbuf[1024] = {0};
   int sendlen = 0;

   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("4t321testcmd\nlpsay: ", sendbuf);

   memset(sendbuf, 0, sizeof(sendbuf)/sizeof(sendbuf[0]));
   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("4t321testcmd\nlpsay: ", sendbuf);
}

void test_procLgModule_notAddFunc(void)
{
   const char recvbuf[] = "testcmd 1 2 3 4t";
   char sendbuf[1024] = {0};
   int sendlen = 0;
   procLgModule(recvbuf, strlen(recvbuf), sendbuf, &sendlen);
   TEST_ASSERT_EQUAL_STRING("no such command\nlpsay: ", sendbuf);
}

