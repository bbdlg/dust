noinst_LIBRARIES=libdust.a
libdust_a_SOURCES= ../commModule/commModule.c ../logModule/logModule.c ../lgModule/lgModule.c ../toolsModule/toolsModule.c ../platform/main.c
libdust_a_LDFLAGS=-DTCP_SERVER_MODE
INCLUDES=-I../toolsModule/ -I../commModule -I../logModule/ -I../errModule/ -I../lgModule/ -I../platform/
DEFS+=-DTCP_SERVER_MODE
DEFS+=-DLOG
DEFS+=-ggdb3
AUTOMAKE_OPTIONS=foreign
