
INC =	-I./ -I../

LIB	=  -L/usr/local/mysql/lib
		
CC      = g++
CFLAGS  = -g -Wall -Werror -Wextra -Wshadow -Wconversion \
	  -std=c++14 -DBOOST_LOG_DYN_LINK

CCFLAGS  = -g -Wall -Werror -std=c++14

LDFLAGS = -Wl,--no-as-needed \
	-lboost_log -lboost_system -lboost_thread \
	-lpthread


.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= ../..
INC += -I$(SRC_DIR)

ZYLIB_DIR = $(SRC_DIR)/zylib
SS_DIR = $(SRC_DIR)/stream_server

SRC_CPP += $(wildcard $(ZYLIB_DIR)/*.cpp)
INC += -I$(ZYLIB_DIR)

SRC_CPP += $(wildcard $(SS_DIR)/*.cpp)
INC += -I$(SS_DIR)

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)



###########################################################################
OUTPUT = server

all:$(OBJS) $(OUTPUT)

server:$(OBJS) Server.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

proto:
	protoc ./msg.proto --cpp_out=./

.cpp.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 
    
.cc.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CCFLAGS) $(INC) -c $< -o $@ 

clean: 
	@echo $(OBJS)
	@rm -f $(OUTPUT)
	@rm -f $(OBJS)
	@find ./ -regex ".*\.o$$" | xargs rm -f
	
