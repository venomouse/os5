//============================================================================
// Name        : os5.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <twitServer.h>

using namespace std;

//commands
#define CONNECT "connect"
#define EXIT "EXIT"
#define FOLLOW "FOLLOW"
#define UNFOLLOW "UNFOLLOW"
#define TWIT "TWIT"
#define DM "DM"
#define BLOCK "BLOCK"
#define WHO "WHO"


#define IS_EQUAL == 0
#define SPACES " \n\r\t"
#define NAME_SEPERATOR "@"

#define FAIL -1;
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535
#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define NDEBUG 0
#define LOCALHOST "127.0.0.1"

int main(int argc, char *argv[]) {
        fd_set master;    // master file descriptor list
        fd_set read_fds;  // temp file descriptor list for select()
        int newfd;        // newly accept()ed socket descriptor
        int fdmax;        // maximum file descriptor number
        struct sockaddr_storage remoteaddr; // client address

        
        FD_ZERO(&master);    // clear the master and temp sets
        FD_ZERO(&read_fds);

   
	if (argc != 2)
	{
		cerr << "Error: usage: twitServer <PORT_NUMBER>" << endl;
                exit(1);
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
	inet_aton(LOCALHOST,&server_addr.sin_addr);
	memset(&(server_addr.sin_zero), '\0', 8);
        int yes = 1;
        if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
        {
            cerr << "Error: failed to setsockopt" << endl;
        }
    
        
	if (bind (server_socket, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0)
	{
		cerr << "Error: failed to bind the socket to the port" << endl;
                exit(1);
	}


	if (listen (server_socket, SOMAXCONN) < 0)
	{
		cerr << "Error: failed to listen to the port" <<endl;
                exit(1);
	}
        FD_SET(server_socket,&master);
        fdmax = server_socket;

        
	if (!NDEBUG)
	{
		cerr << "Binding and listening succeeded" << endl;
	}

	while (1)
	{
		sockaddr_in client_addr;
		char message_buffer[BUFFER_SIZE] = "";
		unsigned int client_length = sizeof (client_addr);
		int nbytes;
                
                read_fds = master; // copy it

                if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
                {
                    cerr<< "Failed to select";
                    exit(1);
                }
                for(int i = 0; i <= fdmax; i++) {
                    if (FD_ISSET(i, &read_fds)) 
                    {
                       if (i == server_socket) {
                        // handle new connections
                        client_length = sizeof remoteaddr;
                        if((newfd = accept(server_socket,(struct sockaddr *)&remoteaddr,&client_length))<0) 
                        {
                            cerr << "Failed to accept" << endl ;
                            exit(1);
                        }else{
                            FD_SET(newfd,&master);
                            if (newfd > fdmax) 
                            {    // keep track of the max
                                fdmax = newfd;
                            }
                        }
                    }else{
                           if ((nbytes = recvAll(i,message_buffer)) >= 0)
                           {
                               parseCommand(i,message_buffer);
                           }
                           else if(nbytes == 0)
                           {
                               close(i);
                               FD_CLR(i,&master);
                           }else
                           {
                               cerr << "Recv error" << endl ; 
                               exit(1);
                           }
                    }
                }
                message_buffer[0] = '\0';
                }
	}
	return 0;
}

int recvAll (int client_socket, char* buffer)
{
    int recv_result;
    char stringSize[3] = "";
    char tempBuffer[BUFFER_SIZE] = "";
    unsigned int intMsgSize =0;
    if ( ( (recv_result = recv (client_socket, stringSize, 3, 0)) <= 0)) 
    {
        return 0;
    }
    intMsgSize = atoi(stringSize);
    while ( ( (recv_result = recv (client_socket, tempBuffer, intMsgSize, 0)) > 0) && 
                        (strlen(buffer)+strlen(tempBuffer) < intMsgSize ) ) 
    {
        strcat(buffer,tempBuffer);
        tempBuffer[0]  = '\0';
    }
    strcat(buffer,tempBuffer);
    if (intMsgSize != strlen(buffer))
    {
        cerr << "Broken message" << endl;
        return 0;
    }
    return intMsgSize;
}



bool parseCommand (int senderFd,const string&  command)
{
    int spaceLocation = command.find_first_of(SPACES);
    
    string operation = command.substr(0,spaceLocation);
    //find first char that is not space after the first space
    string restOfString = command.substr(spaceLocation+1,command.size());
    string args = restOfString.substr(restOfString.find_first_not_of(SPACES),restOfString.size());
    
    string name = getName(senderFd);
    
    if (operation.compare("EXIT") IS_EQUAL)
    {
        disconnect(name);
    }
    else if (operation.compare("FOLLOW") IS_EQUAL )
    {
        follow(name,args);
        
    }else if (operation.compare("UNFOLLOW") IS_EQUAL)
    {
        unFollow(name,args);
        
    }else if (operation.compare("TWIT") IS_EQUAL)
    {   
        twit(name,args);
        
    }else if (operation.compare("DM") IS_EQUAL )
    {   
        directMessage(name,args);
        
    }else if (operation.compare("BLOCK") IS_EQUAL)
    {   
        block(name,args);
    }else if (operation.compare("WHO")IS_EQUAL)
    {   
        who(name);
    };       
    return 0;
}
    
const string& getName(int fd) 
{
    return usersByFd[fd];
}

bool disconnect(const string& sender)
{
    //remove all references from usersByDf , users , and all blocks / followers
    return 0;
}
bool twit(const string& sender,const string& message)
{
    //twit and notify all followers
    return 0;
}

bool follow(const string& follower, const string& toFollow)
{
    //add follower to toFollow followers list
    return 0;
}

bool unFollow(const string& follower, const string& toUnfollow)
{
    //remove follower to toFollow followers list
    return 0;
}
bool directMessage(const string& sender,const string& toAndMessage)
{
    
    //DM if not blocked to toFollow followers list
    return 0;
}
bool block(const string&  blocker,const string&  toBlock)
{
     //add to block list
    return 0;
}
bool who(const string& sender)
{
    //print all users
    return 0;
}