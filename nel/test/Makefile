
INC =	-I./

LIB	=  -L/usr/local/mysql/lib	-lmysqlclient -lboost_system -lpthread
		
CC      = g++
CFLAGS  = -g -Wall -Werror -Wshadow -Wunreachable-code -Wextra -Wconversion -std=c++14
CCFLAGS  = -g -Wall -Werror -std=c++14
LDFLAGS = -Wl,--no-as-needed -lprotobuf

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= ../..

ZYLIB_DIR = $(SRC_DIR)/zylib
NEL_DIR = $(SRC_DIR)/nel

SRC_CPP += $(wildcard $(ZYLIB_DIR)/*.cpp)
INC += -I$(ZYLIB_DIR)

SRC_CPP += $(wildcard $(NEL_DIR)/*.cpp)
INC += -I$(NEL_DIR)

#SRC_CPP += $(wildcard ./*.cpp)
SRC_CC += $(wildcard ./*.cc)

INC += -I$(SRC_DIR)

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)


#OUTPUT = naming_service game_service test_client db_service
OUTPUT = test_bytebuf

all:$(OBJS) $(OUTPUT)

test_bytebuf:$(OBJS) TestByteBuf.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

naming_service:$(OBJS) NamingService.cpp TestDefine.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@
    
game_service:$(OBJS) GameService.cpp TestDefine.cpp GSCallbackManager.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

db_service:$(OBJS) DataBaseService.cpp TestDefine.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_client:$(OBJS) TestClient.cpp TestDefine.cpp
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
	
