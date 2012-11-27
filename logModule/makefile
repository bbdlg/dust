CC=gcc
INCS=-I../toolsModule/
SRCS=./logModule.c ../toolsModule/toolsModule.c
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -Wall -c $(SRCS) ${INCS}

clean:
	@rm -rf *.o

