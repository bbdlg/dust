CC=gcc
SRCS=./logModule.c ../tools/tools.c
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -Wall -c $(SRCS) 

clean:
	@rm -rf *.o

