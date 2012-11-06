CC=gcc
SRCS=./lgModule.c 
OBJS=$(patsubst %.c,%.o,%(SRCS))

all:$(OBJS)
$(OBJS):$(SRCS)
	@cc -c $(SRCS) 

clean:
	@rm -rf *.o

