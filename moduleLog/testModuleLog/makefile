# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ========================================== 

C_COMPILER=gcc
TARGET_BASE1=test_logModule
ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION=.exe
else
	TARGET_EXTENSION=.out
endif
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
SRC_FILES1=../../unity/src/unity.c ./test_logModule.c ./test_logModule_Runner.c
OBJS1=${SRC_FILES1:.c=.o}
INCS=-I../../toolsModule/ -I../../errModule/
INC_DIRS=-Isrc -I../../unity/src -I../../unity/extras/fixture/src/ -I../../unity/extras/fixture/test/ -I../ ${INCS}
SYMBOLS=-DTEST -fno-const-strings -D_LINUX -DLOG

ifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q build\* && del /F /Q $(TARGET1) && del /F /Q $(TARGET2)
else
	CLEANUP = rm -f ${OBJS1} ; rm -f $(TARGET1) ; 
endif

all: clean log ${TARGET1} default

log:
	@cd .. && make all && cd testLogModule

default:
	@echo "start test ..."
	@rm -rf test_logModule_Runner.c
	@$(TARGET1)
	@echo
	@echo "finish test ..."

${TARGET1}:${OBJS1}
	@${C_COMPILER} -o ${TARGET1} ${SYMBOLS} ${OBJS1} ../logModule.o ../toolsModule.o ${INC_DIRS} ${LIB} 

.c.o:
	@${C_COMPILER} -o $@ -c ${INC_DIRS} $<

clean:
	@$(CLEANUP)
	@cd .. && make clean && cd -
	
