#makefile 
SYSTYPE = LINUX2
LIB_PATH=-L../../lib/share -L../../lib/static
INC_DIR = -I. -I../../share -I../../include 
CC      = g++
OPT     = -D__LINUX -O0 -Wall -Wl,--as-needed -shared -fPIC
DEBUG   = -g
DEFS    = $(INC_DIR) -lpthread
CFLAGS  = $(DEBUG) $(OPT) $(LIB_PATH) $(DEFS)
OBJS    = cache_handle.cpp sql_handle.cpp
LIBS    = liblpdb.so

.PHONY: all clean compile install  

all: clean compile install

compile:
	$(CC) $(CFLAGS) -o $(LIBS) $(OBJS)
	@printf "\033[32m SUCCESS \033[0m\n"

install:
	rm -f ../../lib/share/$(LIBS)
	cd ../../include/db/ && find . -type f -name "*.h" | xargs -i rm {}
	cp -rpf $(LIBS) ../../lib/share/$(LIBS)
	cp -rpf *.h ../../include/db

clean:
	rm -f $(LIBS).so
