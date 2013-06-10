CFLAGS = -Wall -I. 

all : twitServer

twitServer: twitServer.cpp twitServer.h
	clang twitServer.cpp $(CFLAGS) -o twitServer
	
clean :
	rm -rf twitServer
