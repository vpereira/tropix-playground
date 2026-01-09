# makefile for lua hierarchy

all:
	LUA=`pwd`; export LUA; (cd src; make)
	LUA=`pwd`; export LUA; (cd clients/lib; make)
	LUA=`pwd`; export LUA; (cd clients/lua; make)

clean:
	LUA=`pwd`; export LUA; (cd src; make clean)
	LUA=`pwd`; export LUA; (cd clients/lib; make clean)
	LUA=`pwd`; export LUA; (cd clients/lua; make clean)
