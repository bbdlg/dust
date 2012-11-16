#include <main.h>

char* homePath = getenv("DUSTHOME");

void procLpMsg(const char* logicName, const int fd, const char* recvbuf, const int recvlen)
{
   char sendbuf[1024] = {0};
   int sendlen = 0;
   procLgModule(recvbuf, recvlen, sendbuf, &sendlen);
   commSend(fd, sendbuf, &sendlen);
}

int main(int argc, char** argv)
{
   int ret,i;
   char tmp[256] = {0};
   struct timeval curTimeval;

   //check running environment
   if(NULL == appVerInfo) {
      WARNINGTIP("it seems that appVerInfo is not set");
   }
   if(NULL == homePath) {
      ERRORTIP("DUSTHOME is not set");
   }
   if(strlen(homePath) > 200) {
      ERRORTIP("$DUSTHOME is too long");
   }
   const char* subdir[] = {"", "bin", "conf", "log", "data"};
   for(i=0; i<sizeof(subdir)/sizeof(subdir[0]); i++) {
      memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
      sprintf(tmp, "%s/%s", homePath, subdir[i]);
      if(access(tmp, F_OK)) {
         ERRORTIP("%s does not exist, try mkdir %s", tmp);
      }
   }
   
   //init logModule
   memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
   sprintf(tmp, "%s/conf/dust.conf", homePath);
   ret = logInit(tmp);
   if(ret) {
      ERRORTIP("%s", moduleErrInfo(comm, ret));
   }

   //init commModule
   memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
   sprintf(tmp, "%s/conf/dust.conf", homePath);
   ret = commInit(tmp);
   if(ret) {
      ERRORTIP("%s", moduleErrInfo(comm, ret));
   }
   ret = commConnect(NULL);
   if(ret) {
      ERRORTIP("%s", moduleErrInfo(comm, ret));
   }

   //init lgModule
   ret = commSetFunc("dustLogModule", &welcomeLp, &procLpMsg);
   if(ret) {
      ERRORTIP("it seems that logicName<dustLogModule> is not set");
   }
 
   //init every function
   initAll();

   //step into endless loop
   while(1) {
      gettimeofday(&curTimeval, NULL);
      checkEvent(curTimeval);
      commProcess();
   }

   return 0;
}
      
   

