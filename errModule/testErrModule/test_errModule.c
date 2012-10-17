#include "../errModule.h"
//#include "../../logModule/logModule.h"
#include "unity.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_moduleErrInfo(void)
{
   enum logErrNo { EA, EB, EC, logMAXERRNO };
   char* logErrInfo[] = {"error a", "error b", "error c"};
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, -1));
   TEST_ASSERT_EQUAL_STRING("error a", moduleErrInfo(log, 0));
   TEST_ASSERT_EQUAL_STRING("error b", moduleErrInfo(log, EB));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO+1));
   //log(LOG_DEBUG, "module:<%s>, errno:<%d>, msg:<%s>", "log", EA, moduleErrInfo(log, EA));
}
