vpath .c ./src
vpath .h ./include

SRC_c := $(wildcard src/*.c)
OBJ_c := $(SRC_c:.c=.o)

SRC_cpp	:= $(wildcard src/*.cpp)
OBJ_cpp := $(SRC_cpp:.cpp=.o)

SRC_cmd	:= $(wildcard src/command/*.c)
OBJ_cmd	:= $(SRC_cmd:.c=.o)

CC	:= gcc
INCDIR := ./include
CFLAGS := -I$(INCDIR)
CFLAGS += -lstdc++


all: main
	

main:$(OBJ_c)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ_c)
