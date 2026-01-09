CC	= cc
CFLAGS	= -O -I .
LIBS	= -lm

OBJS	= hash.o inout.o lex_yy.o opcode.o table.o y_tab.o lua.o iolib.o mathlib.o strlib.o
T	= lua

.virtual all clean

all:	$T

$T:	$OBJS
	$CC -o $T $OBJS $LIBS

hash.o:	hash.c hash.h
	$CC $CFLAGS -c hash.c

inout.o:	inout.c inout.h lua.h
	$CC $CFLAGS -c inout.c

lex_yy.o:	lex_yy.c lua.h lualib.h y_tab.h
	$CC $CFLAGS -c lex_yy.c

opcode.o:	opcode.c opcode.h lua.h
	$CC $CFLAGS -c opcode.c

table.o:	table.c table.h lua.h
	$CC $CFLAGS -c table.c

y_tab.o:	y_tab.c y_tab.h lua.h
	$CC $CFLAGS -c y_tab.c

lua.o:	lua.c lua.h lualib.h
	$CC $CFLAGS -c lua.c

iolib.o:	iolib.c lua.h lualib.h
	$CC $CFLAGS -c iolib.c

mathlib.o:	mathlib.c lua.h lualib.h
	$CC $CFLAGS -c mathlib.c

strlib.o:	strlib.c lua.h lualib.h
	$CC $CFLAGS -c strlib.c

clean:
	rm -f $T $OBJS core
	walk -p "core.*" . | rm -f
