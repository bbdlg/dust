CC=gcc
SRCS=./commModule.c ../tools/tools.c
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -c $(SRCS) -DTCP_CLIENT_MODE 

clean:
	@rm -rf *.o

