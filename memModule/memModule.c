#include "memModule.h"

static FILE* memFp = NULL;

static void outFileFunc(int c)
{
   printf("%c", c);
   if(memFp) {
      fprintf(memFp, "%c", c);
   }
}

int memInit(void)
{
   if(NULL == memFp) {
      char filePath[256] = {0};
      char* homePath = getenv("DFCHOME");
      if(homePath) {
         sprintf(filePath, "%s/log/memwatch.log", homePath);
      }
      else {
         sprintf(filePath, "%s", "memwatch.log");
      }
      memFp = fopen(filePath, "a");
      if(NULL == memFp) {
#ifdef LOG
         log(LOG_ERROR, "fopen %s failed!", filePath);
#else
         DEBUGERROR("fopen %s failed!", filePath);
#endif
         return 1;
      }
      
      /* Set output function */
      mwSetOutFunc(&outFileFunc);

      /* Collect stats on a line number basis */
      mwStatistics( 2 );

      TRACE("hahahhahahahahaha");
   }

   return 0;
}




