
all: twitServer twitClient

twitServer: twitServer.o common.o
	g++ twitServer.o common.o -o twitServer

twitClient: twitClient.o common.o
	g++ twitClient.o common.o -o twitClient

twitServer.o: twitServer.h common.h twitServer.cpp
	g++ -c twitServer.cpp -std=c++0x

twitClient.o: twitClient.h common.h twitClient.cpp
	g++ -c twitClient.cpp 

common.o: common.h common.cpp
	g++ -c common.cpp
