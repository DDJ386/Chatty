vpath .c ./src
vpath .h ./include
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
CC	:= gcc
INCDIR := ./include
CFLAGS := -I$(INCDIR)

all: main clean
	./main

main:$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
