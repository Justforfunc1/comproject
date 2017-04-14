#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../libtools -I../../include
CC      = g++
OPT     = -D__LINUX -O2 -Wall
DEBUG   = -g -std=c++11
DEFS    = $(INC_DIR) -lpthread -ljsoncpp -lboost_filesystem -lboost_system -lboost_regex -lgp_base
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = GPMutex.cpp test.cpp

.PHONY: clean all

all: clean test

compile:

test:
	$(CC) $(CFLAGS) -Wl,-rpath=../../lib/share $(OBJS) -o test 
	@printf "\033[32m SUCCESS \033[0m\n"

clean:
	rm -f *.o *.so *.a test
