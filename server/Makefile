CFLAGS = -Wall -I. -g -std=c++0x

all : twitServer

twitServer: twitServer.cpp twitServer.h common.o
	clang++ twitServer.cpp ../common.o $(CFLAGS) -o twitServer
	
common.o: ../common.h ../common.cpp
	clang++ -c ../common.cpp $(CFLAGS) -o ../common.o
	
clean :
	rm -rf twitServer *.o
