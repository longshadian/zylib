

LIBRDKAFKA_INCLUDE=/home/cgy/libs/librdkafka/src
LIBRDKAFKA_LIB=/home/cgy/libs/librdkafka/src

INC =	-I./ \
        -I$(LIBRDKAFKA_INCLUDE)

LIB	=  -L$(LIBRDKAFKA_LIB) -lrdkafka
		
CC      = g++
CFLAGS  = -g -Wall -Werror -std=c++14
#  -Wshadow -Wunreachable-code -Wextra -Wconversion
LDFLAGS = -Wl,--no-as-needed,-rpath=$(LIBRDKAFKA_LIB) -lpthread

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= ..
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SRC_CC = $(wildcard $(SRC_DIR)/*.cc)
INC += -I$(SRC_DIR)

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)

OUTPUT = test test_ex

all:$(OBJS) $(OUTPUT)

test:$(OBJS) producer.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@

test_ex:$(OBJS) consumer.cpp
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
	
