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

#include "moduleLog.h"

const char* logVersion = "1.0.0";
const char* logCompileDate = __DATE__;
const char* logCompileTime = __TIME__;

const char* logConfFile = "../logMoudle/moduleLog.conf";
const char* defaultRootPathStoreLog = "/log";
const int   defaultSecondsSwitchLog = 60;
const int   defaultKbSwitchLog = 1024;
const char* formatLogDir = "yyyyMMdd";
const char* formatLogFile = "hhmmssuuuuuu.log";
const char* logLevelStr[] = {"DEBUG  ", "INFO   ", "WARNING", "ERROR  "};

char _rootPathStoreLog[256] = {0};
int _secondsSwitchLog = 0;
int _kbSwitchLog = 0;
time_t _curFileCreatedSeconds = 0;
FILE* _fp_log = NULL;

#define generateDirNameAndFileName(dir, file) do {\
   struct tm* pt; \
   struct timeval tv; \
   gettimeofday(&tv, NULL); \
   _curFileCreatedSeconds = tv.tv_sec;\
   pt=gmtime(&_curFileCreatedSeconds);\
   sprintf(dir, "%04d%02d%02d", (1900+pt->tm_year), (1+pt->tm_mon), pt->tm_mday);\
   sprintf(file, "%02d%02d%02d%06ld.log", pt->tm_hour, pt->tm_min, pt->tm_sec, tv.tv_usec);\
}while(0)
 
const char* logErrInfo[logMAXERRNO] = {
   "success",
   "invalid input parameter",
   "file pointer is null",
};

int logInit(const char* configFilePath)
{
   if((NULL == configFilePath) || access(configFilePath, F_OK)) {
      printf("config file path is <%s>, access return %d\n", configFilePath, access(configFilePath, F_OK));
      return LOG_INVALID_INPUT_PARA;
   }
   logConfFile = configFilePath;
   return LOG_SUCCESS;
}

int createLogFile(void)
{
   if(_fp_log) {
      fflush(_fp_log);
      fclose(_fp_log);
      _fp_log = NULL;
   }

   if((strlen(_rootPathStoreLog) == 0) || 
      (_secondsSwitchLog == 0) ||
      (_kbSwitchLog == 0)) 
   {
      char res[256] = {0};
      int len = sizeof(res)/sizeof(res[0]);
      int tmp = 0;
      readValueFromConf_ext(logConfFile, 1, "LogModule", "rootPathStoreLog", res, &len);
      if(strlen(res) != 0) {
         memcpy(_rootPathStoreLog, res, strlen(res));
      }else{
         //fprintf(stdout, "can't get value of rootPathStoreLog from %s, use default value<%s>\n", logConfFile, defaultRootPathStoreLog);
         if(getenv("DFCHOME")) {
            sprintf(_rootPathStoreLog, "%s", getenv("DFCHOME"));
            memcpy(_rootPathStoreLog + strlen(_rootPathStoreLog), defaultRootPathStoreLog, strlen(defaultRootPathStoreLog));
         }
         else {
            memcpy(_rootPathStoreLog, defaultRootPathStoreLog, strlen(defaultRootPathStoreLog));
         }
      }

      len = sizeof(res)/sizeof(res[0]);
      readValueFromConf_ext(logConfFile, 1, "LogModule", "secondsSwitchLog", res, &len);
      if(0 == (tmp=atoi(res))) {
         fprintf(stdout, "can't get value of secondsSwitchLog from %s, use default value<%d>\n", logConfFile, defaultSecondsSwitchLog);
         _secondsSwitchLog = defaultSecondsSwitchLog;
      }else {
         _secondsSwitchLog = tmp;
      }

      len = sizeof(res)/sizeof(res[0]);
      readValueFromConf_ext(logConfFile, 1, "LogModule", "kbSwitchLog", res, &len);
      if(0 == (tmp=atoi(res))) {
         fprintf(stdout, "can't get value of kbSwitchLogfrom %s, use default value<%d>\n", logConfFile, defaultKbSwitchLog);
         _kbSwitchLog = defaultKbSwitchLog;
      }else {
         _kbSwitchLog = tmp;
      }
   }

   char cmd[100]={0};
   sprintf(cmd, "test -d %s", _rootPathStoreLog);
   pid_t status = system(cmd);
   if(WEXITSTATUS(status) == 1)  //_rootPathStoreLog is not a dir
   {
      sprintf(cmd, "mkdir -p %s", _rootPathStoreLog);
      system(cmd);
   }

   char filePath[256]={0}, dir[20]={0}, file[20]={0};
   generateDirNameAndFileName(dir, file);

   sprintf(cmd, "test -d %s/%s", _rootPathStoreLog, dir);
   status = system(cmd);
   if(WEXITSTATUS(status) == 1)  // dir is not a dir
   {
      sprintf(cmd, "mkdir %s/%s", _rootPathStoreLog, dir);
      system(cmd);
   }

   sprintf(filePath, "%s/%s/%s", _rootPathStoreLog, dir, file);
   _fp_log = fopen(filePath, "w+");
   if(NULL == _fp_log)
   {
      printf("fopen %s failed!\n", filePath);
   }

   return LOG_SUCCESS;
}

int initLog(char* rootPathStoreLog, int secondsSwitchLog, int kbSwitchLog)
{
   if((NULL == rootPathStoreLog) ||
         (secondsSwitchLog <= 0) ||
         (kbSwitchLog <= 0))
   {
      return LOG_INVALID_INPUT_PARA;
   }
   memcpy((void*)_rootPathStoreLog, (void*)rootPathStoreLog, strlen(rootPathStoreLog));
   if(_rootPathStoreLog[strlen(_rootPathStoreLog)-1] == '/') {
      _rootPathStoreLog[strlen(_rootPathStoreLog)-1] = 0;
   }
   _secondsSwitchLog = secondsSwitchLog;
   _kbSwitchLog = kbSwitchLog;

   createLogFile();
   return LOG_SUCCESS;
}

int _log(enum LogLevel level, char* function, char* file, int line,  char* msg)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);

   //case not init _fp_log
   if(NULL == _fp_log) {
      createLogFile();
   }

   //switch by size
   long fileLen=ftell(_fp_log);
   if(fileLen >= _kbSwitchLog * 1024) {
      printf("size of file is %ld, larger than %d, call createLogFile()\n", fileLen, _kbSwitchLog*1024);
      createLogFile();
   }

   //switch by time
   if(_curFileCreatedSeconds + _secondsSwitchLog <= tv.tv_sec) {
      printf("current time <%ld> is larger than <%ld + %d>, call createLogFile\n", tv.tv_sec, _curFileCreatedSeconds, _secondsSwitchLog);
      createLogFile();
   }

   if(level != LOG_DEBUG) {
      fprintf(_fp_log, "%ld %06ld %s %s %s %d %s\n", tv.tv_sec, tv.tv_usec, logLevelStr[level], function, file, line, msg);
      fflush(_fp_log);
   }
#ifdef DEBUG
   fprintf(stdout, "%ld %06ld %s %s %s %s %d\n", tv.tv_sec, tv.tv_usec, logLevelStr[level], msg, function, file, line);
#else
   fprintf(stdout, "%ld %06ld %s %s\n", tv.tv_sec, tv.tv_usec, logLevelStr[level], msg);
#endif

   return LOG_SUCCESS;
}

int destoryLog(void)
{
   if(_fp_log)
   {
      fclose(_fp_log);
      _fp_log = NULL;
      return LOG_SUCCESS;
   }
   return LOG_FP_IS_NULL;
}

