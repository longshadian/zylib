

###############################################################################################
###############################################################################################
###############################################################################################
###third party
LOCAL_LIBS=/home/cgy/libs

RDKAFKA_INCLUDE=$(LOCAL_LIBS)/librdkafka-0.11.1/src
RDKAFKA_LIB=$(LOCAL_LIBS)/librdkafka-0.11.1/src

RDKAFKACPP_INCLUDE=$(LOCAL_LIBS)/librdkafka-0.11.1/src-cpp
RDKAFKACPP_LIB=$(LOCAL_LIBS)/librdkafka-0.11.1/src-cpp


###############################################################################################
###############################################################################################
###############################################################################################


INC = 	-I../../ \
	-I$(RDKAFKA_INCLUDE) \
	-I$(RDKAFKACPP_INCLUDE) 
		
LIB	=  ../libknet.a \
	-L$(RDKAFKACPP_LIB) -lrdkafka++ \
	-L$(RDKAFKA_LIB) -lrdkafka \
	 -lboost_system -lpthread

		
CC      = g++
CFLAGS  = -g -Wall -Wextra -Werror -std=c++14
LDFLAGS = -Wl,-rpath=/home/cgy/libs/librdkafka-0.11.1/src-cpp \
	  -Wl,-rpath=/home/cgy/libs/librdkafka-0.11.1/src 

.SUFFIXES:  .o .cpp .cc

.PHONY:clean

#DIR_SRC= .
#SRC_CPP = $(wildcard $(DIR_SRC)/*.cpp)
#SRC_CPP += $(wildcard $(DIR_SRC)/kafka/*.cpp)
#SRC_CPP += $(wildcard $(DIR_SRC)/detail/*.cpp)
#SRC_CPP += $(wildcard $(DIR_SRC)/detail/kafka/*.cpp)

#OBJS = $(SRC_CPP:%.cpp=%.o)
OUTPUT=game lobby

all:$(OUTPUT)

game:game.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@
    
lobby:lobby.cpp
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $@
	
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
	
clean: 
	rm -f $(OUTPUT)
	find ./ -regex ".*\.o$$" | xargs echo
	find ./ -regex ".*\.o$$" | xargs rm -f

