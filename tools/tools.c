#include "tools.h"

int getResultFromSystemCall(const char* pCmd, char* pResult, int* size)
{
   int fd[2];
   if(pipe(fd))   {
      printf("pipe error!\n");
      return -1;
   }

   fflush(stdout);

   //hide stdout
   int bak_fd = dup(STDOUT_FILENO);
   int new_fd = dup2(fd[1], STDOUT_FILENO);

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
   if((NULL == filePath) || (0 == *size)) {
      return -1;
   }
   char cmd[100] = {0};
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

int readValueFromConf_ext(const char* filePath, const int times, const char* field, const char* key, char* res, int* size)
{
   if(   (NULL == filePath) ||
         (NULL == field) ||
         (NULL == key) ||
         (NULL == res) ||
         (NULL == size) ) {
      return TOOLS_INVALID_PARA;
   }

   char cmd[100] = {0};
   sprintf(cmd, "./getValueByFieldAndKey.sh %s %d %s %s", filePath, times, field, key);
   int ret = getResultFromSystemCall(cmd, res, size);
   *size = strlen(res);
   if(0 == times) {
      return atoi(res);
   }
   //fflush(stdout);
   //printf("cmd is <%s>       ret of getResultFromSystemCall is %d, res <%s>, size <%d>\n", cmd, ret, res, *size);
   //fflush(stdout);

   if(0 == *size) {
      return TOOLS_CANNOT_FIND_VALUES;
   }
   return TOOLS_SUCCESS;
}

#if(0)
int main(void)
{
   char res[100] = {0};
   int size = 100;
   readValueFromConf_ext("../commModule/commModule.conf", 0, "TcpClient", "LogicName", res, &size);
   int i = atoi(res);
   printf("i = %d\n", i);
   while(i--) {
      size = 100;
      memset(res, 0, size);
      readValueFromConf_ext("../commModule/commModule.conf", i+1, "TcpClient", "LogicName", res, &size);
      printf("%d <%d-%s>\n", i+1, size, res);
   }
   return 0;
}
#endif
