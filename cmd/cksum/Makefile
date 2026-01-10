#*
#****************************************************************
#*	Makefile para o programa "cksum"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= cksum
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=
OBJS	= cksum.o crc.o print.o sum1.o sum2.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$OBJS
		$CC $DEF -u -s -O -o $PGM $OBJS
		size $PGM
		ls -l $PGM

cksum.o:	cksum.c extern.h
		$CC $DEF -O -c cksum.c

crc.o:		crc.c extern.h
		$CC $DEF -O -c crc.c

print.o:	print.c extern.h
		$CC $DEF -O -c print.c

sum1.o:		sum1.c extern.h
		$CC $DEF -O -c sum1.c

sum2.o:		sum2.c extern.h
		$CC $DEF -O -c sum2.c

lint:
		$CC $DEF -T cksum.c crc.c print.c sum1.c sum2.c

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
		-rm -f $PGM *.o

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
