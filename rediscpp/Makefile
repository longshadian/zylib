

##################################################
HIREDIS_INCLUDE=/usr/local/include/hiredis


##################################################

INC = 	-I./ \
        -I$(HIREDIS_INCLUDE)
		
LIB	= 
		
CC      = g++
CFLAGS  = -g -Wall -Wextra -Werror -std=c++11

.SUFFIXES:  .o .cpp .cc

.PHONY:clean

DIR_SRC= .
SRC_CPP = $(wildcard $(DIR_SRC)/*.cpp)

OBJS = $(SRC_CPP:%.cpp=%.o)

OUTPUT = ./librediscpp.a

$(OUTPUT): $(OBJS)
	ar -rc $(OUTPUT) $(OBJS) 
	
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
	
clean: 
	rm -f $(OUTPUT)
	find ./ -regex ".*\.o$$" | xargs echo
	find ./ -regex ".*\.o$$" | xargs rm -f
