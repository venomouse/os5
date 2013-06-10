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
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535


int main (int argc, char* argv[])
{
	if (argc < 4 || argc > 4)
	{
		cerr << "Error: usage: twitClient <CLIENT NAME> <SERVER ADDRESS> <SERVER PORT>" << endl;
	}

	char* client_name = argv[1];
	char* address = argv[2];
	int port_num = atoi (argv[3]);

	if (port_num < MIN_PORT_NUM || port_num > MAX_PORT_NUM)
	{
		cerr << "Error: illegal port number";
	}

	int client_socket;
	struct sockaddr_in server_addr;
	char message[BUFFER_SIZE];

	client_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		cerr << "Error: Failed to initialize the socket" << endl;
	}

	server_addr.sin_family = AF_INET;
	//The port number is the command parameter
	server_addr.sin_port = htons(port_num);
	//TODO check error
	if (!inet_aton(address,&server_addr.sin_addr))
	{
		cerr << "Error: Wrong IP address" << endl;
	}

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



