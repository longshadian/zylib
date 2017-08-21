
INC =	-I/usr/local/mysql/include		\
		-I./

LIB	=  -L/usr/local/mysql/lib	-lmysqlclient
		
CC      = g++
CFLAGS  = -g -Wall -Werror -std=c++14
#  -Wshadow -Wunreachable-code -Wextra -Wconversion
LDFLAGS = -Wl,--no-as-needed,-rpath=/usr/local/mysql/lib -lpthread

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= ..
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SRC_CC = $(wildcard $(SRC_DIR)/*.cc)
INC += -I$(SRC_DIR)

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)

OUTPUT = test_xx2 test_all test_threads test_safe_string test_convert

all:$(OBJS) $(OUTPUT)

test_xx2:$(OBJS) TestXX2.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_all:$(OBJS) TestAll.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_threads:$(OBJS) TestThreads.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_safe_string:$(OBJS) TestSafeString.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_convert:$(OBJS) TestConvert.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@
    

.cpp.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 
    
.cc.o:
	@echo [  $(shell expr $(shell echo $(OBJS)|sed 's#$@.*$$#$@#'|wc -w) \* 100 / $(words $(OBJS)))%] $(CC) -c $< -o $@ 
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

clean: 
	@echo $(OBJS)
	@rm -f $(OUTPUT)
	@rm -f $(OBJS)
	@find ./ -regex ".*\.o$$" | xargs rm -f
	
