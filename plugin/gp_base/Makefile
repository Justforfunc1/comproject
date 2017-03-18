#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../plugin -I../../include 
CC      = g++
OPT     = -D__LINUX -O2 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -g -std=c++11
DEFS    = $(INC_DIR) -lpthread -ljsoncpp
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = GPException.cpp GPMutex.cpp GPStringUtil.cpp GPTool.cpp GPLogger.cpp GPConfig.cpp GPEncrypt.cpp GPJson.cpp
LIBS    = libgp_base.so

all:
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf *.h ../../include/gp_base/

.PHONY: clean
clean:
	rm -f *.o *.so *.a
