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

#include "../logModule.h"
#include "../../toolsModule/toolsModule.h"
#include "../../errModule/errModule.h"
#include "unity.h"

void setUp(void)
{

}

void tearDown(void)
{
   memset((void*)_rootPathStoreLog, 0, sizeof(_rootPathStoreLog));
   _secondsSwitchLog = 0;
   _kbSwitchLog = 0;
   destoryLog();
   
   system("rm -rf /tmp/log");
}

void test_initLog_initAll(void)
{
   char* path = "/tmp/log/";
   int sec = 123;
   int kb = 456;
   int ret = initLog(path, sec, kb);

   TEST_ASSERT_EQUAL_INT(0, ret);
   TEST_ASSERT_EQUAL_INT8_ARRAY(path, _rootPathStoreLog, strlen(path)-1);
   TEST_ASSERT_EQUAL_INT(sec, _secondsSwitchLog);
   TEST_ASSERT_EQUAL_INT(kb, _kbSwitchLog);
   
   char cmd[100] = {0};
   sprintf(cmd, "rm -rf %s", path);
   system(cmd);
}

void test_initLog_initPath_patherror(void)
{
   int ret = initLog(NULL, 123, 123);

   TEST_ASSERT_EQUAL_INT(LOG_INVALID_INPUT_PARA, ret);
}

void test_initLog_initPath_secerror(void)
{
   char* path = "/tmp/log/";
   int ret = initLog(path, 0, 123);

   TEST_ASSERT_EQUAL_INT(LOG_INVALID_INPUT_PARA, ret);
   
   char cmd[100] = {0};
   sprintf(cmd, "rm -rf %s", path);
   system(cmd);
}

void test_initLog_initPath_kberror(void)
{
   char* path = "/tmp/log";
   int ret = initLog(path, 123, 0);

   TEST_ASSERT_EQUAL_INT(LOG_INVALID_INPUT_PARA, ret);
   
   char cmd[100] = {0};
   sprintf(cmd, "rm -rf %s", path);
   system(cmd);
}

void _test_log(char* logPath, const char* function, int line, enum LogLevel level, const char* msg)
{
   time_t t;
   struct tm* pt;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   t = tv.tv_sec;
   pt=gmtime(&t);
   char logfiledir[100] = {0};
   char logfilename[100] = {0};
   sprintf(logfiledir, "%04d%02d%02d", (1900+pt->tm_year), (1+pt->tm_mon), pt->tm_mday);
   //sprintf(logfilename, "%02d%02d%02d%06d.log", pt->tm_hour, pt->tm_min, pt->tm_sec, tv.tv_usec);
   sprintf(logfilename, "%02d%02d*.log", pt->tm_hour, pt->tm_min);
   //printf("%s/%s/%s\n", logPath, logfiledir, logfilename);

   char cmd[100] = {0};
   sprintf(cmd, "cat %s/%s/%s", logPath, logfiledir, logfilename); 
   FILE* fp = popen(cmd, "r");
   TEST_ASSERT_NOT_NULL(fp);
   char strOnScreen[MAX_LEN_MSG] = {0};
   fgets(strOnScreen, MAX_LEN_MSG, fp);
   fclose(fp);

   char strShouldBe[MAX_LEN_MSG] = {0};
   sprintf(strShouldBe, "%s %s %s %d %s\n", logLevelStr[level], function, __FILE__, line, msg);

   if(level != LOG_DEBUG) {
      char* tmp = NULL;
      tmp = strstr(strstr(strOnScreen, " ")+1, " ") + 1;
      TEST_ASSERT_EQUAL_STRING(tmp, strShouldBe);
   }
}

void test_log_debug(void)
{
   char* logPath = "/tmp/log";
   const char* msg = "this is a debug log record!";
   int ret = initLog(logPath, 600, 1024); //10min or 1Mb switch file
   TEST_ASSERT_EQUAL_INT(0, ret);

   int line = 0;
   HIDE_STDOUT
   log(LOG_DEBUG, "%s", msg); line = __LINE__;
   RESUME_STDOUT

   _test_log(logPath, __FUNCTION__, line, LOG_DEBUG, msg);
}

void test_log_info(void)
{
   char* logPath = "/tmp/log";
   const char* msg = "this is a debug log record!";
   int ret = initLog(logPath, 600, 1024); //10min or 1Mb switch file
   TEST_ASSERT_EQUAL_INT(0, ret);

   int line = 0;
   HIDE_STDOUT
   log(LOG_INFO, "%s", msg); line = __LINE__;
   RESUME_STDOUT

   _test_log(logPath, __FUNCTION__, line, LOG_INFO, msg);
}

void test_log_warining(void)
{
   char* logPath = "/tmp/log";
   const char* msg = "this is a debug log record!";
   int ret = initLog(logPath, 600, 1024); //10min or 1Mb switch file
   TEST_ASSERT_EQUAL_INT(0, ret);

   int line = 0;
   HIDE_STDOUT
   log(LOG_WARNING, "%s", msg); line = __LINE__;
   RESUME_STDOUT

   _test_log(logPath, __FUNCTION__, line, LOG_WARNING, msg);
}

void test_log_error(void)
{
   char* logPath = "/tmp/log";
   const char* msg = "this is a debug log record!";
   int ret = initLog(logPath, 600, 1024); //10min or 1Mb switch file
   TEST_ASSERT_EQUAL_INT(0, ret);

   int line = 0;
   HIDE_STDOUT
   log(LOG_ERROR, "%s", msg); line = __LINE__;
   RESUME_STDOUT

   _test_log(logPath, __FUNCTION__, line, LOG_ERROR, msg);
}

void test_log_noInitLog_noLogConf(void)
{
   const char* logstr = "haha, i am a test string~";
   system("rm -rf ${DFCHOME}/log/*");
   HIDE_STDOUT
   log(LOG_ERROR, logstr);
   RESUME_STDOUT
   char res[200] = {0};
   int size = sizeof(res)/sizeof(res[0]);
   getResultFromSystemCall("cat ${DFCHOME}/log/*/* | awk '{for(i=7;i<=NF;i++){printf $i\" \"}}'", res, &size);
   TEST_ASSERT_EQUAL_STRING(logstr, res);
}

void test_log_noInitLog_haveLogConf(void)
{
   const char* logstr = "haha, i am a test string~";
   system("rm -rf /tmp/logModule.conf");
   system("echo [LogModule] > /tmp/logModule.conf");
   system("echo rootPathStoreLog=\"/tmp/log/sub\" >> /tmp/logModule.conf");
   system("echo secondsSwitchLog=59 >> /tmp/logModule.conf");
   system("echo kbSwitchLog=1023 >> /tmp/logModule.conf");
   logInit("/tmp/logModule.conf");
   HIDE_STDOUT
   log(LOG_ERROR, logstr);
   RESUME_STDOUT

   char res[200] = {0};
   int size = sizeof(res)/sizeof(res[0]);
   getResultFromSystemCall("cat /tmp/log/sub/*/* | awk '{for(i=7;i<=NF;i++){printf $i\" \"}}'", res, &size);
   tearDown(); //call tearDown manually before TEST_IGNORE_MESSAGE
   system("rm -rf /tmp/logModule.conf");

   TEST_ASSERT_EQUAL_STRING(logstr, res);
}

void test_log_switchFilesByTime(void)
{
   TEST_IGNORE_MESSAGE("You must comment this row to finish this test");

   system("rm -rf /tmp/logModule.conf");
   system("echo [LogModule] > /tmp/logModule.conf");
   system("echo rootPathStoreLog=\"/tmp/log/sub\" >> /tmp/logModule.conf");
   system("echo secondsSwitchLog=10 >> /tmp/logModule.conf");
   system("echo kbSwitchLog=50000000 >> /tmp/logModule.conf");

   logInit("/tmp/logModule.conf");

   while(1) {
      usleep(100);
      log(LOG_ERROR, "hallo, i should be appear in /tmp/log/sub/yyyymmdd/*.log");
   }
   //you must press ctrl+c to process this test
}

void test_log_switchFilesBySize(void)
{
   TEST_IGNORE_MESSAGE("You must comment this row to finish this test");
   system("rm -rf /tmp/logModule.conf");
   system("echo [LogModule] > /tmp/logModule.conf");
   system("echo rootPathStoreLog=\"/tmp/log/sub\" >> /tmp/logModule.conf");
   system("echo secondsSwitchLog=1000000 >> /tmp/logModule.conf");
   system("echo kbSwitchLog=500 >> /tmp/logModule.conf");

   logInit("/tmp/logModule.conf");

   while(1) {
      usleep(100);
      log(LOG_ERROR, "hallo, i should be appear in /tmp/log/sub/yyyymmdd/*.log");
   }
   //you must press ctrl+c to process this test
}

void test_logErrInfo_success(void)
{
   int ret = initLog("/tmp/log", 123, 123);

   TEST_ASSERT_EQUAL_INT(LOG_SUCCESS, ret);
   TEST_ASSERT_EQUAL_STRING("success", moduleErrInfo(log, ret));
}

void test_logErrInfo_invalidInputPara(void)
{
   int ret = initLog(NULL, 123, 123);

   TEST_ASSERT_EQUAL_INT(LOG_INVALID_INPUT_PARA, ret);
   TEST_ASSERT_EQUAL_STRING("invalid input parameter", moduleErrInfo(log, ret));
}

void test_logErrInfo_unknowError(void)
{
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, -1));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO));
   TEST_ASSERT_EQUAL_STRING("unknown log module error", moduleErrInfo(log, logMAXERRNO+1));
}

