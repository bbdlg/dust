#ifndef _TOOL_H_
#define _TOOL_H_
/****************************************************************************
 * name:     工具模块
 * version:  v1.0
 * time:     2012-9-26 16:04:37
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    提供了屏蔽/恢复标准输出的两个宏；
 *    提供了从配置文件中读取值的函数；
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* hide and resume print on the screen */
#define HIDE_STDOUT  do { \
   int fd = open("/dev/null",  O_APPEND); \
   if(-1 == fd) {\
      printf("open dev/null error!\n");\
      fflush(stdout);\
      exit(1);\
   }\
   int bak_fd = dup(STDOUT_FILENO);\
   int new_fd = dup2(fd, STDOUT_FILENO);

#define RESUME_STDOUT   \
   dup2(bak_fd, new_fd);\
}while(0);

/* read value from config file
 * content of config file like this:
 *
 *** # key1 is the first test
 *** key1 =   value1
 *** key2 =   value2
 *** # other config content
 *
 */
extern int readValueFromConf(const char* filePath, const char* key, char* res, int size);

#endif
