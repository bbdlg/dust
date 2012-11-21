#include "logModule.h"

const char* logConfFile = "../logMoudle/logModule.conf";
const char* defaultRootPathStoreLog = "/tmp/log";
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
      printf("%s\n", configFilePath);
      printf("access return %d\n", access(configFilePath, F_OK));
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
         fprintf(stderr, "can't get value of rootPathStoreLog from %s, use default value<%s>\n", logConfFile, defaultRootPathStoreLog);
         memcpy(_rootPathStoreLog, defaultRootPathStoreLog, strlen(defaultRootPathStoreLog));
      }

      len = sizeof(res)/sizeof(res[0]);
      readValueFromConf_ext(logConfFile, 1, "LogModule", "secondsSwitchLog", res, &len);
      if(0 == (tmp=atoi(res))) {
         fprintf(stderr, "can't get value of secondsSwitchLog from %s, use default value<%d>\n", logConfFile, defaultSecondsSwitchLog);
         _secondsSwitchLog = defaultSecondsSwitchLog;
      }else {
         _secondsSwitchLog = tmp;
      }

      len = sizeof(res)/sizeof(res[0]);
      readValueFromConf_ext(logConfFile, 1, "LogModule", "kbSwitchLog", res, &len);
      if(0 == (tmp=atoi(res))) {
         fprintf(stderr, "can't get value of kbSwitchLogfrom %s, use default value<%d>\n", logConfFile, defaultKbSwitchLog);
         _kbSwitchLog = defaultKbSwitchLog;
      }else {
         _kbSwitchLog = tmp;
      }
   }

   //printf("_rootPathStoreLog=<%s>, _secondsSwitchLog=<%d>, _kbSwitchLog=<%d>\n", _rootPathStoreLog, _secondsSwitchLog, _kbSwitchLog);

   char cmd[100]={0};
   sprintf(cmd, "test -d %s", _rootPathStoreLog);
   pid_t status = system(cmd);
   if(WEXITSTATUS(status) == 1)  //_rootPathStoreLog is not a dir
   {
      //printf("%s is not a valid directory, now create it!\n", _rootPathStoreLog);
      sprintf(cmd, "mkdir -p %s", _rootPathStoreLog);
      system(cmd);
   }

   char filePath[256]={0}, dir[20]={0}, file[20]={0};
   generateDirNameAndFileName(dir, file);

   sprintf(cmd, "test -d %s/%s", _rootPathStoreLog, dir);
   status = system(cmd);
   if(WEXITSTATUS(status) == 1)  // dir is not a dir
   {
      //printf("%s/%s is not a valid directory, now create it!\n", _rootPathStoreLog, dir);
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
      //printf("_fp_log is null, call createLogFile()\n");
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
   //fprintf(stdout, "%ld %06ld %s %s %s %s %d\n", tv.tv_sec, tv.tv_usec, logLevelStr[level], msg, function, file, line);
   fprintf(stdout, "%ld %06ld %s %s\n", tv.tv_sec, tv.tv_usec, logLevelStr[level], msg);

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

