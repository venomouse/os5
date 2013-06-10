CFLAGS = -Wall -I. -g

all : twitServer

twitServer: twitServer.cpp twitServer.h
	g++ twitServer.cpp $(CFLAGS) -o twitServer
	
clean :
	rm -rf twitServer
