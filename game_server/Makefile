
INC = -I/usr/local/mysql/include		\
	-I/usr/local/include/hiredis	\
	-I./ -I../		\

LIB	=  -lhiredis \
	-L/usr/local/mysql/lib -lmysqlclient \
	-lboost_log \
	-lboost_program_options \
 	-lboost_filesystem \
	-lboost_system \
	-lboost_regex \
	-lboost_thread \
 	-ldl -lpthread
		
CC      = g++
#CFLAGS  = -g -Wall -Wshadow -Wunreachable-code  -Wextra -Wconversion -Wl,--no-as-needed -Werror -std=c++11 -DNDEBUG

#CFLAGS  = -g -Wall -Werror -std=c++11 -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK -DBOOST_LOG_DYN_LINK
CFLAGS  = -g -Wall -Werror -std=c++11 -DBOOST_LOG_DYN_LINK

LDFLAGS = -Wl,--no-as-needed 
#,-rpath=$(BOOST_LIB)

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= .
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SRC_CC = $(wildcard $(SRC_DIR)/proto/*.cc)

############## share dir ##############
SRC_CPP += $(wildcard ../zylib/*.cpp)
INC += -I../zylib

MYSQLCPP=../mysqlcpp
SRC_CPP += $(wildcard $(MYSQLCPP)/*.cpp)
INC += -I$(MYSQLCPP)


############## common dir ##############
COMMON_REDISCPP=./common/rediscpp
SRC_CPP += $(wildcard $(COMMON_REDISCPP)/*.cpp)
INC += -I$(COMMON_REDISCPP)

COMMON_EVENTSERVER=./common/eventserver
SRC_CPP += $(wildcard $(COMMON_EVENTSERVER)/*.cpp)
INC += -I$(COMMON_EVENTSERVER)

COMMON_SIMPLEWEB=./common/SimpleWeb
SRC_CPP += $(wildcard $(COMMON_SIMPLEWEB)/*.cpp)
INC += -I$(COMMON_SIMPLEWEB)
##################################################
PROTO=./proto
INC += -I$(PROTO)

WORLD=./world
SRC_CPP += $(wildcard $(WORLD)/*.cpp)
INC += -I$(WORLD)

OBJ=./obj
SRC_CPP += $(wildcard $(OBJ)/*.cpp)
INC += -I$(OBJ)

DDZ_HTTP=./http
SRC_CPP += $(wildcard $(DDZ_HTTP)/*.cpp)
INC += -I$(DDZ_HTTP)

DATABASE=./database
SRC_CPP += $(wildcard $(DATABASE)/*.cpp)
INC += -I$(DATABASE)

REDIS=./redis
SRC_CPP += $(wildcard $(REDIS)/*.cpp)
INC += -I$(REDIS)

NETWORK=./network
SRC_CPP += $(wildcard $(NETWORK)/*.cpp)
INC += -I$(NETWORK)

GLOBAL_CONF=./global_conf
SRC_CPP += $(wildcard $(GLOBAL_CONF)/*.cpp)
INC += -I$(GLOBAL_CONF)

COMMAND=./command
SRC_CPP += $(wildcard $(COMMAND)/*.cpp)
INC += -I$(COMMAND)

###################################################

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)

OUTPUT = game_server 

all:$(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ $(LIB)

.cpp.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 
    
.cc.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

install:
	cp -f $(OUTPUT) ~/game_server/bin
	cp -rf ./conf ~/game_server/

clean: 
	@echo $(OBJS)
	@rm -f $(OUTPUT)
	@rm -f $(OBJS)
	@find ./ -regex ".*\.o$$" | xargs rm -f
	
