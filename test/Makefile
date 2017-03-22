#LANG=C
SRC_PATH=../test
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib/share -L../lib/static 
SHRLIB_PATH = ../lib/share
STLLIB_PATH = ../lib/static

LIB= -lpthread -lgp_base -ljsoncpp -lboost_filesystem -lboost_system -lboost_regex
SHRLIB = $(SHRLIB_PATH)/libgp_base.so		\
		 $(SHRLIB_PATH)/libprotobuf.so	

STLIB = ../lib/static/libevent.a	\
		../lib/static/libjsoncpp.a	\
		../lib/static/libevent.a

CC=g++
CPPFLAGS= -std=c++11 -Wall -g

OBJS = 

.PHONY:all clean 

all: log_test json_test

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

#event_test:
#	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) libevent_test.cpp $(STLIB) -o $(BIN_PATH)/libevent_test_main
#	@printf "\033[32m JSON_TEST SUCCESS \033[0m\n"

run:
	cd $(BIN)
	./test
	cd $(SRC)
	@printf "\033[32m RUN EXIT!!! \033[0m\n"

clean:
	rm -f *.o

tags:
	rm -rf tags
	touch tags
	find . -iname '*.cpp' -or -iname '*.c' -or -iname '*.h'-or -iname '*.hpp' | xargs ctags --c++-kinds=+p --fields=+iaS --extra=+q --langmap=c++:+.inl -a tags

