CFLAGS = -Wall -I. -g -std=c++0x

all : twitServer

twitServer: twitServer.cpp twitServer.h
	g++ twitServer.cpp $(CFLAGS) -o twitServer
	
clean :
	rm -rf twitServer
