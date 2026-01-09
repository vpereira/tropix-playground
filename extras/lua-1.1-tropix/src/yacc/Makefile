# makefile for lua

LUA= ../..

LIB= $(LUA)/lib
INC= $(LUA)/include

CC= cc
CFLAGS= -O -I $(INC) $(DEFS)
DEFS= -D MAXCODE=64000 -D MAXCONSTANT=1024 -D MAXSYMBOL=1024 -D MAXARRAY=1024

OBJS= hash.o inout.o lex.o opcode.o table.o y.tab.o
SLIB= $(LIB)/liblua.a

libs: $(SLIB)

$(SLIB): y.tab.c $(OBJS)
	mklib -rv $@ $(OBJS)

y.tab.c: lua.stx exscript
	yacc -d lua.stx ; ex y.tab.c <exscript

clean:
	rm -f $(OBJS) $(SLIB)
