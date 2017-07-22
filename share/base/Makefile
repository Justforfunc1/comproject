#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I ./src -I../../share -I../../include 
CC      = g++
OPT     = -D__LINUX -O2 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -g
DEFS    = $(INC_DIR) -lpthread -ljsoncpp
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = src/config.cpp src/logger.cpp src/string_util.cpp src/exception_util.cpp src/json_util.cpp src/md5.cpp src/rc4.cpp src/encrypt.cpp
LIBS    = liblpbase.so

.PHONY: all clean compile install  

all: clean compile install

compile:
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	rm -f ../../lib/share/$(LIBS)
	cd ../../include/base/ && find . -type f -name "*.h" | xargs -i rm {}
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf src/*.h ../../include/base

clean:
	rm -f $(LIBS).so
