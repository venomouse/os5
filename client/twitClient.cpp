/*
 * twitClient.cpp
 *
 *  Created on: Jun 9, 2013
 *      Author: maria
 */

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

using namespace std;

#define MAX_CLIENT_NAME_LENGTH 30
#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define SERVICE_PREFIX_LENGTH 4
#define MAX_MSG_SIZE BUFFER_SIZE - SERVICE_PREFIX_LENGTH
#define FAIL -1
#define SUCCESS 0
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535


int main (int argc, char* argv[])
{
	if (argc != 4 )
	{
		cerr << "Error: usage: twitClient <CLIENT NAME> <SERVER ADDRESS> <SERVER PORT>" << endl;
                exit(1);
	}

	char* clientName = argv[1];
	char* address = argv[2];
	int port_num = atoi (argv[3]);

	checkCommandArguments (clientName, port_num);

	int client_socket;
	struct sockaddr_in server_addr;
	char serverMessageBuffer[MAX_MSG_SIZE];
	string message = "";

	client_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		cerr << "Error: Failed to initialize the socket" << endl;
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	//The port number is the command parameter
	server_addr.sin_port = htons(port_num);
	//TODO check error
	if (!inet_aton(address,&server_addr.sin_addr))
	{
		cerr << "Error: Wrong IP address" << endl;
		exit (1);
	}

	memset(&(server_addr.sin_zero), '\0', 8);

	if (connect (client_socket, (struct sockaddr * )&server_addr, sizeof (server_addr)) < 0)
	{
		cerr << "Error: Failed to connect to the server" << endl;
		exit (1);
	}

	if (sendMessage (client_socket, string(clientName)) < 0)
	{
		cerr << "Error: Failed to send the client's name to the server" << endl;
		exit (1);
	}

	if (recv(client_socket, serverMessageBuffer, MAX_MSG_SIZE, 0) < 0)
	{
		cerr << "Error: Failed to receive authentication from the server" << endl;
		exit (1);
	}

	if (strcmp (serverMessageBuffer, CLIENT_EXISTS_SERVER_MESSAGE) == 0)
	{
		cerr << serverMessageBuffer << endl;
		//TODO disconnect
		exit (1);
	}


	while (1)
	{
		cout << "Enter message to send: " << endl;
                
		getline(cin,message);

		if (sendMessage(client_socket, message) < 0)
		{
			cerr << "Error: Failed to send a message" << endl;
			break;
		}

		if (isExit (message))
		{
			close (client_socket);
			exit (SUCCESS);
		}
	}

}

void checkCommandArguments  (char* clientName, int portNum)
{
	if (portNum < MIN_PORT_NUM || portNum > MAX_PORT_NUM)
	{
		cerr << "Error: illegal port number. " << endl;
		exit(1);
	}

	if (strlen (clientName) > MAX_CLIENT_NAME_LENGTH || (strchr(clientName, ' ') != NULL) ||
			(strchr (clientName, '@') != NULL))
	{
		cerr << "Error: illegal client name." << endl;
		exit(1);
	}

}

int isExit (string message)
{
	string command = beforeSpace (message);
	if (command.compare ("EXIT") == 0)
	{
		return 1;
	}

	return 0;

}





