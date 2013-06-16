//============================================================================
// Name        : os5.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sstream>
#include <utility>
#include "common.h"
#include "twitServer.h"



//commands
#define CONNECT "connect"
#define EXIT "EXIT"
#define FOLLOW "FOLLOW"
#define UNFOLLOW "UNFOLLOW"
#define TWIT "TWIT"
#define DM "DM"
#define BLOCK "BLOCK"
#define WHO "WHO"

//some buggy clients might do this
#define SENT_00 -2
#define FAIL 1
#define SUCCESS 0
#define NO_SUCH_USER -2

#define IS_EQUAL == 0
#define SPACES " \n\r\t"
#define NAME_SEPARATOR "@"
#define NOT !

#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535
#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define NDEBUG 0
#define LOCALHOST "127.0.0.1"

//Messages

//Messages
#define ERROR string("ERROR ")
#define USER_NOT_EXIST  string(" does not exist")
#define PAAMAYIM_NEKUDOTAYIM "::" 
#define DASHDASH "--" 

using std::cerr;
using std::endl;
using std::ostringstream;
using std::make_pair;


int main(int argc, char *argv[]) {
        fd_set master;    // master file descriptor list
        fd_set read_fds;  // temp file descriptor list for select()
        int newfd;        // newly accepted socket descriptor
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
		exit (1);
	}

	struct sockaddr_in server_addr;
	int server_socket;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 1)
	{
		cerr << "Error: failed to initialize a socket" << endl;
		exit(1);
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
                            //receiving the name
                            if ((nbytes = recvAll (newfd, message_buffer)) >=0)
                            {
                            	//if a client already exists
                            	if (connect (newfd, string (message_buffer)) <0)
                            	{
                            		if (sendMessage(newfd, string(CLIENT_EXISTS_SERVER_MESSAGE)))
                            		{
                            			cerr << "Error: failed to send a message to the client" << endl;
                            		}

                            		close (newfd);
                            		continue;
                            	}
                            	else if (sendMessage(newfd, string(CONNECTION_SUCCESSFUL_MESSAGE)) == 0)
                            	{
                            		cerr << "Error: failed to send a message to the client" << endl;
                            	}
                            }
                        }
                    }else{

                           if ((nbytes = recvAll(i,message_buffer)) >= 0)
                           {
                                       parseCommand(i,message_buffer);
                           }
                           else if(nbytes == SENT_00)
                           {
                               //ignore , useless packet
                           }
                           else if(nbytes == 0)
                           {
                               disconnect(getName(i));
                               close(i);
                               FD_CLR(i,&master);
                           }else
                           {
                               cerr << "Recv error" << endl ; 
                               exit(1);
                           }
                           message_buffer[0] = '\0';
                    }
                }
                
                }
	}
	return 0;
}
/*
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
    if (intMsgSize == 0 )
    {
        return SENT_00;
    }
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

*/

bool parseCommand (int senderFd,const string&  command)
{
    string cleanCommand = strip(command);
    
    string operation = beforeSpace(cleanCommand);
    string args = afterSpace(cleanCommand);
    
    
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
    if (NOT userExists(sender))//sender is invalid - something is wrong
    {
        return FAIL;
    }
    for(int fd : users.at(sender).followers)
    {
        sendToClient(getName(fd),message);
    }
    return SUCCESS;
}

bool follow( string& follower,  string& toFollow)
{
    if (NOT userExists(follower))//sender is invalid - something is wrong
    {
        return FAIL;
    }
    if (NOT userExists(toFollow)) //cant follow - no such user
    {
        ostringstream error ;
        error << ERROR << toFollow << USER_NOT_EXIST;
        sendToClient(follower,error.str());
        return FAIL;
    }
    users.at(toFollow).followers.insert(users.at(follower).sockfd);
    return SUCCESS;
}

bool unFollow( string& follower,  string& toUnfollow)
{
    if (NOT userExists(follower)) //sender is invalid - something is wrong
    {
        return FAIL;
        
    }
    if (NOT userExists(toUnfollow)) //cant follow - no such user
    {
        
        sendToClient(follower,toUnfollow.append(USER_NOT_EXIST));
        return FAIL;
    }
    users.at(toUnfollow).followers.erase(users.at(follower).sockfd);
    return SUCCESS;
}
bool directMessage(string& sender,string& toAndMessage)
{
    if (NOT userExists(sender)) //sender is invalid - something is wrong
    {
        return FAIL;
    }
    
    string to = beforeSpace(toAndMessage);
    if (NOT userExists(to)) //cant follow - no such user
    {
        sendToClient(sender,to.append(USER_NOT_EXIST));
        return FAIL;
    }
    if (users.at(to).blocked.find(getFd(to)) != users.at(to).blocked.end())
    {
        sendToClient(sender,createErrorMsg(to,USER_NOT_EXIST));
        return FAIL;
    }
    ostringstream message;
    message << getTimeString() 
            << PAAMAYIM_NEKUDOTAYIM
            << sender
            << NAME_SEPARATOR 
            << to 
            << DASHDASH
            << afterSpace(toAndMessage);
    
    sendToClient(to,message.str());
    return SUCCESS;
}
bool block(const string&  blocker,const string&  toBlock)
{
    if (NOT userExists(blocker))
    {
        return FAIL;
    }
    if (NOT userExists(toBlock))
    {
        sendToClient(blocker,createErrorMsg(toBlock,USER_NOT_EXIST));
        return FAIL;
    }
    users.at(blocker).blocked.insert(getFd(toBlock));
    users.at(toBlock).followers.erase(getFd(toBlock));
    
    return 0;
}
bool who(const string& sender)
{
    //print all users
    return 0;
}



bool connect(int senderFd, const string& name)
{      
    string lowCaseName = toLower(name);
    if(userExists(lowCaseName))
    {
        cerr << CLIENT_EXISTS_SERVER_MESSAGE <<endl;
        return FAIL;
    }
    usersByFd[senderFd] = name;
    users.insert(make_pair(lowCaseName,User(name,senderFd)));
    return SUCCESS;
}

bool sendToClient(const string& name ,const string& message )
{
    //find user 
    //send message
    //return - success?
    return SUCCESS;
}
bool userExists(const string& userNameLowerCase )
{
    return(users.find(userNameLowerCase) != users.end());
}



int getFd(const string& userName)
{
    string lowName = toLower(userName);    
    if (NOT userExists(lowName))
    {
        return NO_SUCH_USER;
    }
    return users.at(lowName).sockfd; 
}

User& getUser(const string& userName)
{
    string lowName = toLower(userName);
    return users.at(lowName);
}


/*
string getTimeString()
{
    ostringstream ret;
    time_t rawtime;
    struct tm * ptm;
    time ( &rawtime );
    ptm = gmtime ( &rawtime );
    ret << ptm->tm_hour <<  ":" <<ptm->tm_min;
    return ret.str();
}

string createErrorMsg(const string& userName, const string& errorType)
{
        ostringstream error ;
        error << " " <<ERROR << userName << " "  << errorType;
        return error.str();
}


    
string beforeSpace(const string& stringWithSpace )
{
    int spaceLocation = stringWithSpace.find_first_of(SPACES);
    string before = stringWithSpace.substr(0,spaceLocation);
    return strip(before);
}

    
string afterSpace(const string& stringWithSpace )
{
    int spaceLocation = stringWithSpace.find_first_of(SPACES);
    string after = stringWithSpace.substr(spaceLocation+1,stringWithSpace.size());
    return strip(after);
}


string strip(const string& stringWithSpaces )
{
    int firstSpace = stringWithSpaces.find_first_of(SPACES);
    int lastSpace = stringWithSpaces.find_last_of(SPACES);
    return stringWithSpaces.substr(firstSpace,lastSpace);
}


string toLower(const string &str)
{
    string copy =str;
    transform(copy.begin(),copy.end(),copy.begin(),::tolower);
    return copy;
}
*/
