CFLAGS = -Wall -I. -g

all : twitServer

twitServer: twitClient.cpp twitClient.h
	g++ twitClient.cpp $(CFLAGS) -o twitClient
	
clean :
	rm -rf twitClient
