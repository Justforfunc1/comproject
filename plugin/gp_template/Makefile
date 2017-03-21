#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../plugin -I../../include 
CC      = g++
OPT     = -D__LINUX -O2 -Wall
DEBUG   = -g -std=c++11
DEFS    = $(INC_DIR) -lpthread -lboost_regex -l
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = GPMutex.cpp test.cpp

all: test

compile:

test:
	$(CC) $(CFLAGS) $(OBJS) -o test 
	@printf "\033[32m SUCCESS \033[0m\n"


.PHONY: clean
clean:
	rm -f *.o *.so *.a
