//============================================================================
// Name        : os5.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define FAIL -1;
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535
#define BUFFER_SIZE 140
#define NDEBUG 0

int main(int argc, char *argv[]) {

	if (argc > 2)
	{
		cerr << "Error: usage: twitServer <PORT_NUMBER>" << endl;
	}

	int port_number = atoi (argv [1]);

	if (port_number < MIN_PORT_NUM || port_number > MAX_PORT_NUM)
	{
		cerr << "Error: illegal port number" << endl;
	}

	struct sockaddr_in server_addr;
	int server_socket;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 1)
	{
		cerr << "Error: failed to initialize a socket" << endl;
	}

	server_addr.sin_family = AF_INET;
	//The port number is the command parameter
	server_addr.sin_port = htons(atoi (argv [1]));
	//TODO check error
	inet_aton("127.0.0.1",&server_addr.sin_addr);
	memset(&(server_addr.sin_zero), '\0', 8);

	if (bind (server_socket, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0)
	{
		cerr << "Error: failed to bind the socket to the port" << endl;
	}


	//TODO - add setsockopt option?

	if (listen (server_socket, SOMAXCONN) < 0)
	{
		cerr << "Error: failed to listen to the port" <<endl;
	}

	if (!NDEBUG)
	{
		cerr << "Binding and listening succeeded" << endl;
	}

	while (1)
	{
		sockaddr_in client_addr;
		char message_buffer[BUFFER_SIZE];
		unsigned int client_length = sizeof (client_addr);
		int client_socket;

		if ((client_socket = accept (server_socket, (struct sockaddr *)&client_addr, &client_length)) < 0)
		{
			cerr << "Failed to accept" << endl;
			break;
		}

		int recv_result;
		while ((recv_result = recv (client_socket, message_buffer, BUFFER_SIZE, 0)) > 0)
		{
			//TODO it's more complicated than that
			cout << message_buffer << endl;
		}
		if (recv_result == 0)
		{
			cout << "Client disconnected" << endl;
		}

	}



	return 0;
}
