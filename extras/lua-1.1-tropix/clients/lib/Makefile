# makefile for lualib

LUA= ../..

INC= $(LUA)/include
LIB= $(LUA)/lib

CC= cc
CFLAGS= -O -I $(INC) $(DEFS)
DEFS=
DEFS=

OBJS= iolib.o mathlib.o strlib.o
SLIB= $(LIB)/liblualib

.virtual libs clean

libs: $(SLIB)

$(SLIB): $(OBJS)
	mkdir -f $(SLIB) >/dev/null 2>&1; echo
	mklib -rf $(SLIB) $(OBJS)

iolib.o:	iolib.c
	$(CC) $(CFLAGS) -c iolib.c

mathlib.o:	mathlib.c mathlib.h
	$(CC) $(CFLAGS) -c mathlib.c

strlib.o:	strlib.c strlib.h
	$(CC) $(CFLAGS) -c strlib.c

clean:
	rm -f $(OBJS)
	rmtree -f $(SLIB)
