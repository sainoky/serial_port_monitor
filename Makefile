BIN	:= fancy_cui
OBJS	:= fancy_cui.o serial_util.o
CFLAGS	:= -O2 -Wall -Wextra
CC	:= gcc

all: $(BIN)

clean:
	- rm $(BIN) *.o

fancy_cui: $(OBJS)

fancy_cui.o: \
	fancy_cui.c serial_util.h
serial_util.o: \
	serial_util.c
