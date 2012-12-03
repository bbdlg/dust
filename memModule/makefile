CC=gcc
INCS=-I../platform/ -I../errModule/ -I../logModule/ -I../commModule/ -I../lgModule/ -I../toolsModule/
SRCS=./memModule.c ./memwatch.c
OBJS=$(patsubst %.c,%.o,%(SRCS))
FLGS=-DMEMWATCH -DMW_STDIO

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -Wall -c $(SRCS) ${INCS} ${FLGS}

clean:
	@rm -rf *.o

