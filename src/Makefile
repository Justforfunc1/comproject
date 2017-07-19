#LANG=C
SRC_PATH=../src
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib
SHRLIB_PATH = ../lib

LIB= -lpthread -lboost_system -lboost_thread -lboost_regex -lrt 
SHRLIB = $(SHRLIB_PATH)/ \

STLIB = 

CC=g++
CPPFLAGS=-Wall -g 

OBJS = server_state.o

all: clean compile main

compile:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c server_state.cpp -o server_state.o

main:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(OBJS) $(STLIB) main_run.cpp -o $(BIN_PATH)/main
	
	#mv $@ $(BIN_PATH)
	#rm -f *.o 
	@printf "\033[32m SUCCESS \033[0m\n"

run:
		cd $(BIN)
		./test
		cd $(SRC)
		@printf "\033[32m RUN EXIT!!! \033[0m\n"

clean:
		rm -f *.o



