INC = 	-I./
		
LIB	= 
		
CC      = g++
CFLAGS  = -g -Wall -Wl,--no-as-needed -std=c++11 -Werror
#CFLAGS  = -g -Wall -Werror -std=c++11

.SUFFIXES:  .o .cpp .cc

.PHONY:all install clean

DIR_SRC= .
SRC_CPP = $(wildcard $(DIR_SRC)/*.cpp)

OBJS = $(SRC_CPP:%.cpp=%.o)

OUTPUT = ./libzylib.a

all:$(OBJS)
	ar -rc $(OUTPUT) $(OBJS) 

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ -o $@ $(LIB)

test:$(OBJS) test.cc
	$(CC) $(CFLAGS) $(INC) $^ -o $@ $(LIB)
	
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

.cc.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
	
clean: 
	rm -f $(OUTPUT) ./test
	find ./ -regex ".*\.o$$" | xargs echo
	find ./ -regex ".*\.o$$" | xargs rm -f
