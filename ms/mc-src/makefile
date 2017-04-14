SRC=../src
BIN=../bin
INC_PATH=-I. -I../include -I../src
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

OBJS = MQLogger.o MQPath.o MsgPack.o MQConnector.o MQSession.o MQControl.o mqclient2.o TargetParser.o

all: clean comm ms mc
	@echo -e "\033[32;40m MQ SUCCESS ! \033[0m"

comm:
	$(CC) $(CPPFLAGS) -c $(SRC)/MQLogger.cpp -o MQLogger.o
	$(CC) $(CPPFLAGS) -c $(SRC)/MQPath.cpp -o MQPath.o
	$(CC) $(CPPFLAGS) -c $(SRC)/MsgPack.cpp -o MsgPack.o
	$(CC) $(CPPFLAGS) -c $(SRC)/MQConnector.cpp -o MQConnector.o
	$(CC) $(CPPFLAGS) -c $(SRC)/MQSession.cpp -o MQSession.o
	$(CC) $(CPPFLAGS) -c $(SRC)/MQControl.cpp -o MQControl.o
	$(CC) $(CPPFLAGS) -c $(SRC)/mqclient2.cpp -o mqclient2.o
	$(CC) $(CPPFLAGS) -c TargetParser.cpp -o TargetParser.o
mc:
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS) mc.cpp -o $(BIN)/mc2 
ms:
	$(CC) $(CPPFLAGS) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(SHRLIB) $(OBJS) ms.cpp -o $(BIN)/ms2
clean:
	rm -f *.o
	rm -rf $(BIN)/mc2 $(BIN)/ms2
