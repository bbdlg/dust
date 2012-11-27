CC=gcc
INCS=-I../platform/ -I../errModule/ -I../logModule/ -I../commModule/ -I../lgModule/ -I../toolsModule/
SRCS=./lgModule.c 
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -Wall -c $(SRCS) ${INCS}

clean:
	@rm -rf *.o

