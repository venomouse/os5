CFLAGS = -c -std=c++0x -Wall

all: twitServer twitClient

twitServer: twitServer.o common.o
	g++ twitServer.o common.o -o twitServer

twitClient: twitClient.o common.o
	g++ twitClient.o common.o -o twitClient

twitServer.o: twitServer.h common.h twitServer.cpp
	g++  twitServer.cpp $(CFLAGS)

twitClient.o: twitClient.h common.h twitClient.cpp
	g++  twitClient.cpp $(CFLAGS)

common.o: common.h common.cpp
	g++  common.cpp $(CFLAGS)

