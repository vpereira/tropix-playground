# makefile for lua interpreter

LUA= ../..

BIN= $(LUA)/bin
INC= $(LUA)/include
LIB= $(LUA)/lib

CC= cc
CFLAGS= -O -I $(INC)

OBJS= lua.o
T=$(BIN)/lua

.virtual all clean

all:	$T

$(BIN):
	mkdir -f $(BIN) >/dev/null 2>&1; echo

$T: $(BIN) $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIB)/liblua $(LIB)/liblualib -lm

lua.o:	lua.c
	$(CC) $(CFLAGS) -c lua.c

clean:
	rm -f $(OBJS)
	rmtree -f $T
