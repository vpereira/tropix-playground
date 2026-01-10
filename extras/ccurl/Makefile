#*
#****************************************************************
#*	Makefile for ccurl - simple HTTP client		*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= ccurl
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

.virtual	all clean lint cp cmp cmpq cmpqcp unld man modif prman

all:		$PGM

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:
		$CC $DEF -T $PGM.c

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

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr
