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

using namespace std;

#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define MAX_MSG_SIZE BUFFER_SIZE - 4
#define FAIL -1
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535


int main (int argc, char* argv[])
{
	if (argc != 4 )
	{
		cerr << "Error: usage: twitClient <CLIENT NAME> <SERVER ADDRESS> <SERVER PORT>" << endl;
                exit(1);
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
	string message = "";

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
                
		getline(cin,message);
                if(message.size() >= MAX_MSG_SIZE - 4)
                {
                    message = message.substr(0,MAX_MSG_SIZE-1);
                }
                //convert length to string , code snippet from http://www.cplusplus.com/articles/D9j2Nwbp/
                unsigned int msgSize = message.size();
                string msgLen = static_cast<ostringstream*>( &(ostringstream() << msgSize) )->str(); 
                if (msgSize < 10) 
                {
                    msgLen = string("00").append(msgLen);
                }else if (msgSize < 100)
                {
                    msgLen = string("0").append(msgLen);
                }
                message = msgLen.append(message);

		if (send (client_socket, message.c_str(), msgSize + 4 ,0) < 0)
		{
			cerr << "Error: Failed to send a message" << endl;
			break;
		}
	}

}



