/*
 * twitClient.cpp
 *
 *  Created on: Jun 9, 2013
 *      Author: maria
 */

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define BUFFER_SIZE 140
#define FAIL -1


int main (int argc, char* argv[])
{
	int client_socket;
	struct sockaddr_in server_addr;
	char message[BUFFER_SIZE];

	client_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		cerr << "Failed to initialize the socket" << endl;
	}

	server_addr.sin_family = AF_INET;
	//The port number is the command parameter
	server_addr.sin_port = htons(atoi (argv [1]));
	//TODO check error
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(server_addr.sin_zero), '\0', 8);

	if (connect (client_socket, (struct sockaddr * )&server_addr, sizeof (server_addr)) < 0)
	{
		cerr << "Error: Failed to connect to the server" << endl;
	}

	while (1)
	{
		cout << "Enter message to send: " << endl;
		cin >> message;

		if (send (client_socket, message, strlen (message),0) < 0)
		{
			cerr << "Error: Failed to send a message" << endl;
			break;
		}
	}

}



