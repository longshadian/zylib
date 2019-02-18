

###############################################################################################
###############################################################################################
###############################################################################################
###third party
LOCAL_LIBS=/home/cgy/libs

ZYLIB_INCLUDE=$(LOCAL_LIBS)/tcwlibs/zylib
ZYLIB_LIB=$(LOCAL_LIBS)/tcwlibs/zylib

###############################################################################################
###############################################################################################
###############################################################################################


INC = -I./

LIB	= -lboost_log \
	-lboost_filesystem \
	-lboost_system \
	-lpthread
		
CC      = g++
#CFLAGS  = -g -Wall -Wshadow -Wunreachable-code  -Wextra -Wconversion -Wl,--no-as-needed -Werror -std=c++11 -DNDEBUG
CFLAGS  = -g -Wall -Wshadow -Werror -Wextra -Wconversion -std=c++14 -DBOOST_LOG_DYN_LINK
CFLAGS_PROTO  = -g -Wall -Wshadow -Werror -std=c++14
LDFLAGS = 
          

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= .
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SRC_CC = $(wildcard $(SRC_DIR)/proto/*.cc)

##################################################
WORLD=./world
SRC_CPP += $(wildcard $(WORLD)/*.cpp)

NET=./net
SRC_CPP += $(wildcard $(NET)/*.cpp)

###################################################

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)

OUTPUT = frontend_service

all:$(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ $(LIB)

.cpp.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 
	
    
.cc.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS_PROTO) $(INC) -c $< -o $@ 

protocol:
	cd ../proto/tools; ./generate-server.sh
    
#install:
#	cp -f $(OUTPUT) /home/cgy/szmj_server/szmj/bin/
    

clean:
	@echo $(OBJS)
	@rm -f $(OUTPUT)
	@rm -f $(OBJS)
	@find ./ -regex ".*\.o$$" | xargs rm -f
	
