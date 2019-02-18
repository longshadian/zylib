
##########################################################
##########################################################
##########################################################
LIBRDKAFKA_INCLUDE=/home/cgy/libs/librdkafka-0.11.3/src
LIBRDKAFKA_LIB=/home/cgy/libs/librdkafka-0.11.3/src

LIBRDKAFKAcpp_INCLUDE=/home/cgy/libs/librdkafka-0.11.3/src-cpp
LIBRDKAFKACPP_LIB=/home/cgy/libs/librdkafka-0.11.3/src-cpp

##########################################################
##########################################################
##########################################################



INC =	-I./ \
        -I$(LIBRDKAFKA_INCLUDE) \
        -I$(LIBRDKAFKAcpp_INCLUDE)

LIB	=  -L$(LIBRDKAFKA_LIB) -L$(LIBRDKAFKACPP_LIB) -lrdkafka++ -lrdkafka
		
CC      = g++
CFLAGS  = -g -Wall -Werror -std=c++14
#  -Wshadow -Wunreachable-code -Wextra -Wconversion
LDFLAGS = -Wl,--no-as-needed,-rpath=$(LIBRDKAFKA_LIB),-rpath=$(LIBRDKAFKACPP_LIB) -lpthread

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

SRC_DIR	= .
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SRC_CC = $(wildcard $(SRC_DIR)/*.cc)
INC += -I$(SRC_DIR)

OBJS = $(SRC_CPP:%.cpp=%.o)
OBJS += $(SRC_CC:%.cc=%.o)

OUTPUT = test_consumer test_producer

all:$(OBJS) $(OUTPUT)

test_consumer:TestConsumer.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@
    
test_producer:TestProducer.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@


#test_consumer:TestConsumer.cpp
	#$(CC) $(CFLAGS) $(INC) $^ -I/home/cgy/libs/librdkafka/src-cpp -L/home/cgy/libs/librdkafka/src-cpp $(LIB) -lrdkafka++ -lrdkafka -Wl,-rpath=/home/cgy/libs/librdkafka/src-cpp,-rpath=/home/cgy/libs/librdkafka/src -o $@

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
	
