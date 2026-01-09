# makefile for lua

LUA= ..

LIB= $(LUA)/lib
INC= $(LUA)/include

CC= cc
CFLAGS= -O -I $(INC) $(DEFS)
DEFS= -D MAXCODE=64000 -D MAXCONSTANT=1024 -D MAXSYMBOL=1024 -D MAXARRAY=1024

OBJS= hash.o inout.o lex.o opcode.o table.o y.tab.o
SLIB= $(LIB)/liblua

.virtual libs clean

libs: $(SLIB)

$(SLIB): $(OBJS)
	mkdir -f $(SLIB) >/dev/null 2>&1; echo
	mklib -rf $(SLIB) $(OBJS)

hash.o:	hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

inout.o:	inout.c inout.h
	$(CC) $(CFLAGS) -c inout.c

lex.o:	lex.c
	$(CC) $(CFLAGS) -c lex.c

opcode.o:	opcode.c opcode.h
	$(CC) $(CFLAGS) -c opcode.c

table.o:	table.c table.h
	$(CC) $(CFLAGS) -c table.c

y.tab.o:	y.tab.c y.tab.h
	$(CC) $(CFLAGS) -c y.tab.c

clean:
	rm -f $(OBJS)
	rmtree -f $(SLIB)
