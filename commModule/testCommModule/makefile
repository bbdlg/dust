# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ========================================== 

C_COMPILER=gcc
TARGET_BASE1=test_commModule
ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION=.exe
else
	TARGET_EXTENSION=.out
endif
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
SRC_FILES1=../../unity/src/unity.c ./test_commModule.c ./test_commModule_Runner.c
OBJS1=${SRC_FILES1:.c=.o}
INC_DIRS=-Isrc -I../../unity/src -I../../unity/extras/fixture/src/ -I../../unity/extras/fixture/test/ -I../
SYMBOLS=-DTEST -fno-const-strings -D_LINUX 

ifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q build\* && del /F /Q $(TARGET1) && del /F /Q $(TARGET2)
else
	CLEANUP = rm -f ${OBJS1} ; rm -f $(TARGET1) ; 
endif

all: clean comm lg ${TARGET1} default

comm:
	@cd .. && make all && cd testCommModule

lg:
	@cd ../../lgModule/ && make all && cd ../commModule/testCommModule

default:
	@echo "start test ..."
	@rm -rf test_commModule_Runner.c
	@$(TARGET1)
	@echo "finish test ..."

${TARGET1}:${OBJS1} 
	@${C_COMPILER} -o ${TARGET1} ${SYMBOLS} ${OBJS1} ../commModule.o ../tools.o ../../lgModule/lgModule.o ${OBJS_COMM} ${INC_DIRS} ${LIB} 

.c.o:
	@${C_COMPILER} -o $@ -c ${INC_DIRS} $<

clean:
	@cd .. && make clean && cd testCommModule
	@$(CLEANUP)
	
