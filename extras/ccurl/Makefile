#*
#****************************************************************
#*	Makefile for ccurl - simple HTTP client		*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= ccurl
PROT	= a+x
CC	= cc
DEF	=

.virtual	all clean cp cmp

all:		$PGM

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

clean:
		-rm -f $PGM
		-rm -f core
		-walk -p "core.*" . | rm -f

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR
