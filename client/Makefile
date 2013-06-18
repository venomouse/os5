CFLAGS = -Wall -I. -g -std=c++0x

all : twitClient

twitClient: twitClient.cpp twitClient.h common.o
	clang++ twitClient.cpp ../common.o $(CFLAGS) -o twitClient
	
common.o: ../common.h ../common.cpp
	clang++ -c ../common.cpp $(CFLAGS) -o ../common.o
clean :
	rm -rf twitClient *.o
