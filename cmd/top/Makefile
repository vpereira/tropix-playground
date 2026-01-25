#*
#****************************************************************
#*	Makefile para o programa "top"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= top
PROT	= a+x,u+sm
MAN	= cmd
CC	= cc
DEF	= -D PC

.virtual	lint cp cmp cmpq cmpqcp unld man modif zipto zipfrom

$PGM:		$PGM.o ttytb.o
		$CC $DEF -u -s -o $PGM $PGM.o ttytb.o -lcurses
		size $PGM
		ls -l $PGM

$PGM.o:		$PGM.c
		$CC $DEF -O -c -o $PGM.o $PGM.c

ttytb.o:	../ps/mkttytb.c
		$CC $DEF -s -u -o mkttytb ../ps/mkttytb.c
		mkttytb -v
		as -o ttytb.o ttytb.s
		rm -f ttytb.s mkttytb

lint:
		$CC $DEF -T $PGM.c

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
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield
