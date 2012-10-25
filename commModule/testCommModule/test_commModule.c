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
   initComm("../commModule.conf");
   TEST_ASSERT_EQUAL_INT(24, getSizeOfGLinkMap());
}
