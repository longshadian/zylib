INC = 	-I../
		
LIB	= 
		
CC      = g++
CFLAGS  = -g -Wall -Wextra -Werror -std=c++14

.SUFFIXES:  .o .cpp .cc

.PHONY:clean

DIR_SRC= .
SRC_CPP = $(wildcard $(DIR_SRC)/*.cpp)
SRC_CPP += $(wildcard $(DIR_SRC)/kafka/*.cpp)
SRC_CPP += $(wildcard $(DIR_SRC)/detail/*.cpp)

OBJS = $(SRC_CPP:%.cpp=%.o)

OUTPUT = ./libknet.a

$(OUTPUT): $(OBJS)
	ar -rc $(OUTPUT) $(OBJS) 
	
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
	
clean: 
	rm -f $(OUTPUT)
	find ./ -regex ".*\.o$$" | xargs echo
	find ./ -regex ".*\.o$$" | xargs rm -f

