#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../share -I../../include 
CC      = g++
OPT     = -D__LINUX -O2 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -std=c++11 -g
DEFS    = $(INC_DIR) -lpthread -ljsoncpp -lboost_filesystem -lboost_system -lboost_regex 
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = ./Utils/GPException.cpp ./Template/GPMutex.cpp ./Utils/GPStringUtil.cpp ./Utils/GPTool.cpp ./Log/GPLogger.cpp ./Config/GPConfig.cpp ./Utils/GPEncrypt.cpp ./Utils/GPJson.cpp ./Utils/GPFile.cpp ./Auth/GPMd5.cpp ./Auth/GPRc4.cpp
LIBS    = libgpbase.so

.PHONY: all clean compile install  

all: clean compile install

compile:
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	rm -f ../../lib/share/$(LIBS)
	cd ../../include/Base/ && find . -type f -name "*.h" | xargs -i rm {}
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf ./Auth/*.h ../../include/Base/Auth/
	cp -rpf ./Common/*.h ../../include/Base/Common/
	cp -rpf ./Config/*.h ../../include/Base/Config/
	cp -rpf ./Log/*.h ../../include/Base/Log/
	cp -rpf ./Template/*.h ../../include/Base/Template/
	cp -rpf ./Utils/*.h ../../include/Base/Utils/

clean:
	rm -f $(LIBS).so
