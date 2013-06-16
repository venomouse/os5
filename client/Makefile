CFLAGS = -Wall -I. -g

all : twitServer

twitServer: twitClient.cpp twitClient.h common.o
	g++ twitClient.cpp ../common.o $(CFLAGS) -o twitClient
	
common.o: ../common.h ../common.cpp
	g++ -c common.cpp $(CFLAGS) -o ../common.o
clean :
	rm -rf twitClient *.o
