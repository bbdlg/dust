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

#include "moduleTools.h"

#ifdef WIN32
#else
int getResultFromSystemCall(const char* pCmd, char* pResult, int* size)
{
   int bak_fd=0,new_fd=0;
   int fd[2] = {0};

   if(pipe(fd))   {
      printf("pipe error!\n");
      return -1;
   }

   fflush(stdout);

   //hide stdout
   bak_fd = dup(STDOUT_FILENO);
   new_fd = dup2(fd[1], STDOUT_FILENO);

   //the output of `pCmd` writes into fd[1]
   system(pCmd);
   fprintf(stdout, "B"); //write a byte to prevent read() block when `pCmd` block
   fflush(stdout);
   memset(pResult, 0, *size);
   read(fd[0], pResult, (*size)-1);
   //fprintf(stderr, "read %d bytes from fd[0], str is <%s>\n", strlen(pResult), pResult);
   pResult[strlen(pResult)-1] = 0;
   if(strlen(pResult)) {
      pResult[strlen(pResult)-1] = 0;
   }
   //fprintf(stderr, "now read %d bytes from fd[0], str is <%s>\n", strlen(pResult), pResult);
   
   //resume stdout
   dup2(bak_fd, new_fd);

   //close pipe fd
   close(fd[0]);
   close(fd[1]);
   
   fflush(stdout);
   //fprintf(stderr, "after %s, -%s-, strlen is %d, size is %d\n", __FUNCTION__, pResult, strlen(pResult), size);

   return 0;
}

int readValueFromConf(const char* filePath, const char* key, char* res, int* size)
{
   char cmd[100] = {0};
   if((NULL == filePath) || (0 == *size)) {
      return -1;
   }
   //sprintf(cmd, "cat %s 2>/dev/null | grep -m 1 -E \"^%s\" | cut -d= -f2 | sed 's/[[:space:]]*//g'", filePath, key); 
   sprintf(cmd, "cat %s | grep -m 1 -E \"^%s\" | cut -d= -f2 | sed 's/[[:space:]]*//g'", filePath, key); 
   memset(res, 0, *size);
   if(0) //why??
   {
      return getResultFromSystemCall(cmd, res, size);
   }
   else
   {
      int ret = getResultFromSystemCall(cmd, res, size);
      return ret;
   }
}
#endif

//return how many times field appears, if 0 == times;
//return TOOLS_CANNOT_FIND_VALUES, if can't find field in config file;
//return TOOLS_SUCCESS, if find field, and copy section to 'section'.
int getSection(const char* filePath, const int times, const char* field, char* section)
{
   char field_cmp[MAX_LEN_ROW] = {0};
   char row[MAX_LEN_ROW];
   char flag_start_copy = 0;
   FILE* fp = NULL;
   int curTimes = 0;

   sprintf(field_cmp, "[%s]", field);

   if(NULL == (fp = fopen(filePath, "r"))) {
      return TOOLS_CANNOT_OPEN_CONF;
   }

   memset(row, 0, MAX_LEN_ROW);
   while(fgets(row, MAX_LEN_ROW, fp)) {
      if(('[' == row[0]) && (1 == flag_start_copy)) {
         flag_start_copy = 2; //copy finished
      }
      if(0 == memcmp(field_cmp, row, strlen(field_cmp))) {
         curTimes++;
         if(times == curTimes) {
            flag_start_copy = 1; //start to copy
         }
      }
      if(1 == flag_start_copy) {
         if(MAX_LEN_SECTION <= (strlen(section) + strlen(row))) {
            return TOOLS_SECTIONLEN_OVERFLOW;
         }
         memcpy(section+strlen(section), row, strlen(row));
      }
      else if(2 == flag_start_copy) {
         return TOOLS_SUCCESS;
      }
      memset(row, 0, MAX_LEN_ROW);
   }

   if(0 == times) {
      return curTimes;
   }
   return TOOLS_SUCCESS;
}

int getValueOfKey(const char* section, const char* key, char* res, int* size)
{
   char* pos = NULL;
   int f_copy = 0;
   int f_copy_space = 0;
   char ctmp[MAX_LEN_ROW] = {0};
   char *pStartCopy=NULL, *pStopCopy=NULL;

   if((NULL == section) || (NULL == key) || (NULL == size)) {
      return TOOLS_INVALID_PARA;
   }

   sprintf(ctmp, "\n%s", key);
   pStartCopy = strstr(section, ctmp);
   if(NULL == pStartCopy) {
      return TOOLS_CANNOT_FIND_VALUES;
   }
   pStartCopy++;
   pStopCopy = strchr(pStartCopy, '\n');
   if(NULL == pStopCopy) {
      pStopCopy = strchr(pStartCopy, '\0');
      if(NULL == pStopCopy) {
         return TOOLS_CANNOT_FIND_VALUES;
      }
   }

   memset(ctmp, 0, MAX_LEN_ROW);
   memcpy(ctmp, pStartCopy, (pStopCopy - pStartCopy));
   if((pStopCopy - pStartCopy) >= (MAX_LEN_ROW - 1)) {
      return TOOLS_ROWLEN_OVERFLOW;
   }

#define start_copy(c)((c != '\t') && (c != '\n') && (c != ' ') && (c != '=') )
#define stop_copy(c)(c == '\0')
#define is_space(c) (('\t' == c) || (' ' == c) || ('\n' == c))
   pos = ctmp + strlen(key);
   f_copy = 0;
   f_copy_space = 0;
   *size = 0;
   while (1) {
      if(start_copy(*pos)) {
         f_copy = 1;
      }
      if(stop_copy(*pos)) {
         f_copy = 0;
         break;
      }
      if (f_copy) {
         if('"' == *pos) {
            f_copy_space++;
         }
         if(0 == f_copy_space%2) {
            if(is_space(*pos)) break;
         }
         res[(*size)++] = *pos;
      }
      pos++;
   }

   if(('"' == res[0]) && ('"' == res[strlen(res)-1])) {
      int i=0;
      for(i=0; i<*size; i++)
      {
         res[i] = res[i+1];
      }
      res[*size - 2] = 0;
   }

   return TOOLS_SUCCESS;
}

int readValueFromConf_ext(const char* filePath, const int times, const char* field, const char* key, char* res, int* size)
{
   char section[MAX_LEN_SECTION] = {0};
   int ret = 0;

   if(0 == times) {
      return getSection(filePath, times, field, section);
   }

   ret = getSection(filePath, times, field, section);
   if(TOOLS_SUCCESS != ret) {
      return ret;
   }

   return getValueOfKey(section, key, res, size);
}

void hexdump(const void* _data, int size, const char* info) 
{
   int offset=0, i=0, n=0;
   const char* data = (const char*)_data;
   if(info) {
      printf("%s\n", info); 
   }
   while (offset < size) {
      printf("%06x  ", offset);
      n = size - offset;
      if (n > 16) {
         n = 16;
      }

      for (i = 0; i < 16; ++i) {
         if (i == 8) {
            printf(" ");
         }

         if (offset + i < size) {
            printf("%02x ", data[offset + i]);
         } else {
            printf("   ");
         }
      }
      printf(" ");

      for (i = 0; i < n; ++i) {
         if (isprint(data[offset + i])) {
            printf("%c", data[offset + i]);
         } else {
            printf(".");
         }
      }
      printf("\n");
      offset += 16;
   }
}

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
   time_t clock;
   struct tm tm;
   SYSTEMTIME wtm;
   GetLocalTime(&wtm);
   tm.tm_year     = wtm.wYear - 1900;
   tm.tm_mon     = wtm.wMonth - 1;
   tm.tm_mday     = wtm.wDay;
   tm.tm_hour     = wtm.wHour;
   tm.tm_min     = wtm.wMinute;
   tm.tm_sec     = wtm.wSecond;
   tm. tm_isdst    = -1;
   clock = mktime(&tm);
   tp->tv_sec = clock;
   tp->tv_usec = wtm.wMilliseconds * 1000;
   return (0);
}
#endif
