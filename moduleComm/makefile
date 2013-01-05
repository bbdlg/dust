CC=gcc
INCS=-I../toolsModule/ -I../platform/ -I../lgModule/
SRCS=./commModule.c ../toolsModule/toolsModule.c
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -Wall -c $(SRCS) ${INCS} -DTCP_CLIENT_MODE -DTCP_SERVER_MODE

clean:
	@rm -rf *.o

