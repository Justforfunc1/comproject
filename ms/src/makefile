#LANG=C
SRC_PATH=../src
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib
SHRLIB_PATH = ../lib

LIB= -lpthread -lboost_system -lboost_thread
SHRLIB = $(SHRLIB_PATH)/libqpidmessaging.so \
		 $(SHRLIB_PATH)/libqpidclient.so    \
		 $(SHRLIB_PATH)/libqpidcommon.so    \
		 $(SHRLIB_PATH)/libqpidtypes.so
STLIB = 

CC=g++
CPPFLAGS=-Wall -g $(INC_PATH) $(LIB_PATH) 

OBJS = MQLogger.o MQPath.o MsgPack.o MQConnector.o MQSession.o MQControl.o mqclient2.o

all: clean compile output

compile:
	$(CC) $(CPPFLAGS) -fPIC -c MQLogger.cpp -o MQLogger.o
	$(CC) $(CPPFLAGS) -fPIC -c MQPath.cpp -o MQPath.o
	$(CC) $(CPPFLAGS) -fPIC -c MsgPack.cpp -o MsgPack.o
	$(CC) $(CPPFLAGS) -fPIC -c MQConnector.cpp -o MQConnector.o
	$(CC) $(CPPFLAGS) -fPIC -c MQSession.cpp -o MQSession.o
	$(CC) $(CPPFLAGS) -fPIC -c MQControl.cpp -o MQControl.o
	$(CC) $(CPPFLAGS) -fPIC -c mqclient2.cpp -o mqclient2.o

output:
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  send.cpp -o $(BIN_PATH)/send2 
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  recv.cpp -o $(BIN_PATH)/recv2
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  dialog.c -o $(BIN_PATH)/dialog2
	#$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  dialog2.c -o $(BIN_PATH)/dialog2
	#$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  getMsg.cpp -o $(BIN_PATH)/getMsg
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -fPIC -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS)  -shared -o $(SRC_PATH)/libmqclient2.so

	#mv $@ $(BIN_PATH)
	#rm -f *.o cs
	@printf "\033[32m SUCCESS \033[0m\n"

clean:
	rm -f *.o *.so