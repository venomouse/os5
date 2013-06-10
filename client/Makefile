CFLAGS = -Wall -I. 

all : twitServer

twitServer: twitClient.cpp twitClient.h
	clang twitClient.cpp $(CFLAGS) -o twitClient
	
clean :
	rm -rf twitClient
