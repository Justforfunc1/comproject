#LANG=C
SRC_PATH=../src
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib
SHRLIB_PATH = ../lib

LIB= -lpthread -lboost_system -lboost_thread -lboost_regex -lrt -llpsys
SHRLIB = $(SHRLIB_PATH)/libqpidmessaging.so \
		 $(SHRLIB_PATH)/libqpidclient.so    \
		 $(SHRLIB_PATH)/libqpidcommon.so    \
		 $(SHRLIB_PATH)/libqpidtypes.so     \
		 $(SHRLIB_PATH)/libqpidtypes.so		\
		 $(SHRLIB_PATH)/libmqclient2.so		\
		 $(SHRLIB_PATH)/liblpsys.so
STLIB = ../lib/libdb_cxx-5.3.a ../lib/libhiredis.a

CC=g++
CPPFLAGS=-Wall -g 

OBJS = IpSearch.o Encrypt.o Tool.o PacketPool.o ServerListener.o ServerState.o CacheHandle.o DBHandle.o DataLoader.o MemoryObject.o MsgDealler.o MsgReciver.o IndexCore.o IndexBuilder.o IndexSearcher.o BatchLoader.o FilterChain.o FilterContext.o FilterManager.o PacketParseFilter.o AdMatchFilter.o TemplateAssemblyFilter.o PacketOrganizingFilter.o HttpResponseFilter.o 
INDEX_OBJS = IndexDataLoader.o IndexBuilder.o IndexCore.o IndexSearcher.o

all: clean compile test

compile:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c Tool.cpp -o Tool.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c Encrypt.cpp -o Encrypt.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IpSearch.cpp -o IpSearch.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c PacketPool.cpp -o PacketPool.o
	
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c ServerListener.cpp -o ServerListener.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c ServerState.cpp -o ServerState.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c CacheHandle.cpp -o CacheHandle.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c DBHandle.cpp -o DBHandle.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c DataLoader.cpp -o DataLoader.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c MemoryObject.cpp -o MemoryObject.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c MsgDealler.cpp -o MsgDealler.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c MsgReciver.cpp -o MsgReciver.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c BatchLoader.cpp -o BatchLoader.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexCore.cpp -o IndexCore.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexBuilder.cpp -o IndexBuilder.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexSearcher.cpp -o IndexSearcher.o
	
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c FilterChain.cpp -o FilterChain.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c FilterContext.cpp -o FilterContext.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c FilterManager.cpp -o FilterManager.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c PacketParseFilter.cpp -o PacketParseFilter.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c AdMatchFilter.cpp -o AdMatchFilter.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c TemplateAssemblyFilter.cpp -o TemplateAssemblyFilter.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c PacketOrganizingFilter.cpp -o PacketOrganizingFilter.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c HttpResponseFilter.cpp -o HttpResponseFilter.o
	
ams:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(OBJS) $(STLIB) MainRun.cpp -o $(BIN_PATH)/ams 
	
	#mv $@ $(BIN_PATH)
	#rm -f *.o 
	@printf "\033[32m SUCCESS \033[0m\n"

index:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexDataLoader.cpp -o IndexDataLoader.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexBuilder.cpp -o IndexBuilder.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexCore.cpp -o IndexCore.o
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c IndexSearcher.cpp -o IndexSearcher.o

		$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) $(INDEX_OBJS) $(STLIB) IndexMain.cpp -o $(BIN_PATH)/test_index 
			
run:
		cd $(BIN)
		./test
		cd $(SRC)
		@printf "\033[32m RUN EXIT!!! \033[0m\n"

cnt_tool:
		$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) cnt_tool.cpp -o $(BIN_PATH)/cnt_tool

db_tool:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) CacheHandle.o DBHandle.o db_tool.cpp ../lib/libdb_cxx-5.3.a ../lib/libhiredis.a -o $(BIN_PATH)/db_tool
	
ip_tool:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) IpSearch.o ip_tool.cpp -o $(BIN_PATH)/ip_tool

clean:
		rm -f *.o test db_tool cnt_tool



