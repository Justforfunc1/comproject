#makefile 
SYSTYPE = LINUX2
INC_DIR = -I. -I../../plugin
CC      = g++
OPT     = -D__LINUX -O2 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -g -std=c++11
DEFS    = $(INC_DIR) -lpthread
CFLAGS  = $(DEBUG) $(OPT) $(DEFS)
OBJS    = GPMutex.cpp
LIBS    = libgp_template.so

all:
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf *.h ../../include/gp_base/

.PHONY: clean
clean:
	rm -f *.o *.so *.a
