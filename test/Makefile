#LANG=C
SRC_PATH=../test
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib/share -L../lib/static 
SHRLIB_PATH = ../lib/share
STLLIB_PATH = ../lib/static

LIB= -lpthread -lmysqlclient -llpbase -llpdb -lhiredis -ljsoncpp -levent
SHRLIB = $(SHRLIB_PATH)/liblpbase.so		\
		 $(SHRLIB_PATH)/libmysqlclient.so	\
		 $(SHRLIB_PATH)/libprotobuf.so		\
		 $(SHRLIB_PATH)/libhiredis.so		\
		 $(SHRLIB_PATH)/liblpdb.so	

STLIB = ../lib/static/libevent.a	\
		../lib/static/libjsoncpp.a	\
		../lib/static/libmysqlclient.a	\
		../lib/static/liblua.a	\
		../lib/static/libevent.a

CC= gcc
CPPFLAGS= -Wall -g 

OBJS = 

.PHONY:all clean 

all: log_test json_test event_test thread_test cache_test sql_test

compile:
#	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c epoll_connection.cpp -o epoll_connection.o
	
	
log_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) log_test.cpp -o log_test_mian
	@printf "\033[32m LOG_TEST SUCCESS \033[0m\n"

json_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) json_test.cpp $(STLIB) -o json_test_main
	@printf "\033[32m JSON_TEST SUCCESS \033[0m\n"

thread_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) thread_test.cpp $(STLIB) -o thread_test_main
	@printf "\033[32m THREAD_TEST SUCCESS \033[0m\n"

event_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) event_test.cpp $(STLIB) -o event_test_main
	@printf "\033[32m EVENT_TEST SUCCESS \033[0m\n"

cache_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) cache_test.cpp $(STLIB) -o cache_test_main
	@printf "\033[32m CACHE_TEST SUCCESS \033[0m\n"

sql_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH)  sql_test.cpp $(STLIB) -o sql_test_main
	@printf "\033[32m SQL_TEST SUCCESS \033[0m\n"

lua_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH)  lua_test.cpp $(STLIB) -o lua_test_main
	@printf "\033[32m LUA_TEST SUCCESS \033[0m\n"


run:
	cd $(BIN)
	./test
	cd $(SRC)
	@printf "\033[32m RUN EXIT!!! \033[0m\n"

clean:
	rm -f *.o log_test_mian json_test_main thread_test_main event_test_main cache_test_main sql_test_main lua_test_main

tags:
	rm -rf tags
	touch tags
	find . -iname '*.cpp' -or -iname '*.c' -or -iname '*.h'-or -iname '*.hpp' | xargs ctags --c++-kinds=+p --fields=+iaS --extra=+q --langmap=c++:+.inl -a tags

