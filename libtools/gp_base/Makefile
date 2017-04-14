#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../libtools -I../../include 
CC      = g++
OPT     = -D__LINUX -O2 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -std=c++11 -g
DEFS    = $(INC_DIR) -lpthread -ljsoncpp -lboost_filesystem -lboost_system -lboost_regex 
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = GPException.cpp GPMutex.cpp GPStringUtil.cpp GPTool.cpp GPLogger.cpp GPConfig.cpp GPEncrypt.cpp GPJson.cpp GPFile.cpp GPMd5.cpp GPRc4.cpp
LIBS    = libgp_base.so

.PHONY: clean all install 
all: clean
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	rm -f ../../lib/share/$(LIBS)
	rm -f ../../include/gp_base/*
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf *.h ../../include/gp_base/

clean:
	rm -f *.o *.so *.a
