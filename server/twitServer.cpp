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
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sstream>
#include <vector>
#include <utility>
#include "common.h"
#include "twitServer.h"

#define LOG_NAME "twitServer.log"

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
#define SUCCESS 0
#define NO_SUCH_USER -2

#define IS_EQUAL == 0
#define SPACES " \n\r\t"
#define NAME_SEPARATOR "@"
#define NOT !
#define NAME_TWIT_SEPERATOR " - "
#define MAX_NAME_LENGTH 30
#define MIN_PORT_NUM 1025
#define MAX_PORT_NUM 65535
#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define LOCALHOST "127.0.0.1"

//Messages

//Messages
#define ALREADY_CONNECTED_MSG "Error: Already connected\n"
#define ILLEGAL_COMMAND_MSG "Error: Illegal command"
#define FROM " from "
#define CLIENT_NAME_TOO_LONG "Error:  client name is too long\n"
#define BROKEN_DM_MSG "Error: Malformed DM.\n"
#define BLOCKED_RESPONSE_MSG "Message Blocked.\n"
#define FOLLOWED_MSG "\t followed "
#define UNFOLLOWED_MSG "\t unfollowed "
#define BLOCKED_MSG "\t blocked "
#define WHO_MSG "\t who.\n"
#define TWITED_MSG "\t twitted "
#define CONNECT_MSG "\t connected.\n";
#define DISCONNECT_MSG "\t disconnected.\n"
#define DM_MSG "\t direct messaged "
#define CANT_DELETE "Error: Cant delete user - does not exist"
#define FD_NOT_EXIST_MSG "Error: No such file descriptor.\n"
#define USER_NOT_EXIST  string(" does not exist.\n")
//A homage to php
#define PAAMAYIM_NEKUDOTAYIM "::"  
#define DASHDASH "--" 

std::fstream logFile;
using std::cerr;
using std::cout;
using std::endl;
using std::ostringstream;
using std::pair;
using std::make_pair;
using std::fstream;
using std::stringstream;


int main(int argc, char *argv[]) {
    
    
    string message;

    int newfd; // newly accepted socket descriptor
    int fdmax; // maximum file descriptor number
    struct sockaddr_storage remoteaddr; // client address


    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);
    FD_SET (STDIN_FILENO, &master);


    if (argc != 2) {
        cerr << "Error: usage: twitServer <PORT_NUMBER>" << endl;
        exit(1);
    }
    

    int port_number = atoi(argv [1]);

    if (port_number < MIN_PORT_NUM || port_number > MAX_PORT_NUM) {
        cerr << "Error: illegal port number" << endl;
        exit(1);
    }

    struct sockaddr_in server_addr;
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 1) {
        cerr << "Error: failed to initialize a socket" << endl;
        exit(1);
    }
    
    
    server_addr.sin_family = AF_INET;
    //The port number is the command parameter
    server_addr.sin_port = htons(atoi(argv [1]));
    //TODO check error
    inet_aton(LOCALHOST, &server_addr.sin_addr);
    memset(&(server_addr.sin_zero), '\0', 8);
    int yes = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        cerr << "Error: failed to setsockopt" << endl;
    }


    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
        cerr << "Error: failed to bind the socket to the port" << endl;
        exit(1);
    }


    if (listen(server_socket, SOMAXCONN) < 0) {
        cerr << "Error: failed to listen to the port" << endl;
        exit(1);
    }
    FD_SET(server_socket, &master);
    fdmax = server_socket;


    if (!NDEBUG) {
        cerr << "Binding and listening succeeded" << endl;
    }
    
    if (openLog())
    {
    	cerr << "Log failed to open" << endl;
    	exit (1);
    }
    

    while (1) {
        sockaddr_in client_addr;
        char message_buffer[BUFFER_SIZE] = "";
        unsigned int client_length = sizeof (client_addr);
        int nbytes;

        read_fds = master; // copy it

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            cerr << "Error: Failed to select";
            exit(1);
        }
        if (FD_ISSET (STDIN_FILENO, &read_fds))
        {
            getline(std::cin,message);
            cout << toLower(message).compare("exit");
            cout.flush();
            if (toLower(message).compare("exit") IS_EQUAL){
                for(pair<int,string> userPair: usersByFd){
                        close(userPair.first);
                        cout <<userPair.first <<endl;
                        log(userPair.second.append(DISCONNECT_MSG));
                }           
                logFile.close();
                exit(0);
            }

        }
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_socket) {
                    // handle new connections
                    client_length = sizeof remoteaddr;
                    if ((newfd = accept(server_socket, (struct sockaddr *) &remoteaddr, &client_length)) < 0) {
                        cerr << "Error: Failed to accept" << endl;
                        exit(1);
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) { // keep track of the max
                            fdmax = newfd;
                        }
                    }
                } else {

                    if ((nbytes = recvAll(i, message_buffer)) > 0) {
                        parseCommand(i, message_buffer);
                    } else if (nbytes == SENT_00) {
                        //ignore , useless packet
                    } else if (nbytes == 0) {
                        disconnect(i);
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        cerr << "Error: Failed to receive from a client" << endl;
                        exit(1);
                    }
                    message_buffer[0] = '\0';
                }
            }

        }
    }
    return 0;
}


int parseCommand(int senderFd, const string& command) {
    string cleanCommand = strip(command);

    string operation = beforeSpace(cleanCommand);
    string args = afterSpace(cleanCommand);


    string name = getName(senderFd);
    if (operation.compare("CONNECT") IS_EQUAL){
        connectClient( senderFd,args);
    }else if (operation.compare("EXIT") IS_EQUAL) {
        disconnect(senderFd);
    } else if (operation.compare("FOLLOW") IS_EQUAL) {
        follow(senderFd, args);

    } else if (operation.compare("UNFOLLOW") IS_EQUAL) {
        unFollow(senderFd, args);

    } else if (operation.compare("TWIT") IS_EQUAL) {
        twit(senderFd, args);

    } else if (operation.compare("DM") IS_EQUAL) {
        directMessage(senderFd, args);

    } else if (operation.compare("BLOCK") IS_EQUAL) {
        block(senderFd, args);
    } else if (operation.compare("WHO")IS_EQUAL) {
        who(senderFd);
    }else{
        stringstream logMsg;
        logMsg << ILLEGAL_COMMAND_MSG << operation << FROM << name<<".\n";
        cerr << logMsg.str(); cerr.flush();
        log(logMsg.str());
        sendMessage(senderFd,logMsg.str());
        return FAIL;
    }
    return SUCCESS;
}



string getName(int fd) {
	if (!NDEBUG)
	{
		//cerr << "user " << usersByFd[fd] << endl;
	}
    return usersByFd[fd];
}

int disconnect(int senderFd) {

  /*  if (NOT fdExist(senderFd))
    {
        log(CANT_DELETE);
        return FAIL;
    }*/

    //delete the user from users structures

    for( pair<string,User> pair: users)
    {
    	pair.second.followers.erase(senderFd);
    	pair.second.blocked.erase(senderFd);
    }
    string name = getName(senderFd);
    users.erase(name);
    usersByFd.erase(senderFd);

    log(name.append(DISCONNECT_MSG));
    close (senderFd);
    FD_CLR (senderFd, &master);
    return true;
}

int twit(int senderFd, const string& message) {
    if (NOT fdExist(senderFd))//sender is invalid - something is wrong
    {
        log(FD_NOT_EXIST_MSG);
        return FAIL;
    }
    ostringstream toTwit;
    string senderName = getName(senderFd);
    string truncatedMsg = message.substr(0,140);
    toTwit << getTimeString() << PAAMAYIM_NEKUDOTAYIM << senderName  << NAME_TWIT_SEPERATOR <<truncatedMsg <<"\n";
    for (int fd : users.at(toLower(senderName)).followers) {
        sendMessage(fd, toTwit.str());
    }

    senderName.append(TWITED_MSG);
    senderName.append(truncatedMsg);
    log(senderName.append(".\n"));
    return SUCCESS;
}

int follow(int senderFd, string& toFollow) {
    if (NOT fdExist(senderFd))//sender is invalid - something is wrong
    {
        log(FD_NOT_EXIST_MSG);
        return FAIL;    
    }
    stringstream logMsg;
    string senderName = getName (senderFd);
    logMsg<<senderName<<FOLLOWED_MSG << toFollow<< ".\n";
    log(logMsg.str());
    if (NOT userExists(toFollow)) //cant follow - no such user
    {
        ostringstream error;
        error << ERROR << toFollow << USER_NOT_EXIST;
        sendMessage(senderFd,error.str());
        log(error.str());
        return FAIL;
    }
    //If sender followed someone after blocking him, the block is erased
    if (users.at(toLower(senderName)).blocked.find(getFd(toFollow)) != users.at(toLower(senderName)).blocked.end())
    {
    	users.at(toLower(senderName)).blocked.erase(getFd(toFollow));
    }
    users.at(toLower(toFollow)).followers.insert(senderFd);
    return SUCCESS;
}

int unFollow(int senderFd, string& toUnfollow) {
    stringstream logMsg;
    logMsg <<getName(senderFd)<< UNFOLLOWED_MSG << toUnfollow << ".\n";
    log(logMsg.str());
    if (NOT fdExist(senderFd))//sender is invalid - something is wrong
    {
        log(FD_NOT_EXIST_MSG);
        return FAIL;    
    }
    if (NOT userExists(toUnfollow)) //cant follow - no such user
    {
        log(toUnfollow.append(USER_NOT_EXIST));
        sendMessage(senderFd,toUnfollow.append(USER_NOT_EXIST));
        return FAIL;
    }
    users.at(toLower(toUnfollow)).followers.erase(senderFd);
    return SUCCESS;
}

int directMessage(int senderFd, string& toAndMessage) {
    int nsLocation;
    if (NOT fdExist(senderFd)) //sender is invalid - something is wrong
    {
        log(FD_NOT_EXIST_MSG);
        return FAIL;
    }
    if((nsLocation = toAndMessage.find(NAME_SEPARATOR)) < 0){
        cerr << BROKEN_DM_MSG;
        cerr.flush();
        sendMessage(senderFd, BROKEN_DM_MSG);
    	log(BROKEN_DM_MSG);
        return FAIL;
    }
    string to = toLower(toAndMessage.substr(0,nsLocation));
    string content = toAndMessage.substr(nsLocation+1,140+nsLocation);
    stringstream logMsg;
    logMsg << getName(senderFd)<<DM_MSG << to << "\t " << content << "\n";
    log(logMsg.str());
    
    logMsg.clear();
    logMsg.str(to);
    if (NOT userExists(to)) //cant follow - no such user
    {
        logMsg << USER_NOT_EXIST;
        sendMessage(senderFd, logMsg.str());
        log(logMsg.str());
        return FAIL;
    }
    //DEBUG
    for (int fd : users.at(toLower(to)).blocked){
        cout << fd<< endl;
    }
    if (users.at(toLower(to)).blocked.find(senderFd) != users.at(to).blocked.end()) {
        sendMessage(senderFd,BLOCKED_RESPONSE_MSG);
        log(BLOCKED_MSG);
        return FAIL;
    }
    stringstream message;
    message << getTimeString()
            << PAAMAYIM_NEKUDOTAYIM
            << getName(senderFd)
            << NAME_SEPARATOR
            << to
            << DASHDASH
            << content
            << "\n";

    sendMessage(getFd(to),message.str());
    return SUCCESS;
}

int block(int blockerFD, const string& toBlock) {
    
    string blockerName = getName(blockerFD);
    if (NOT fdExist(blockerFD)) {
        return FAIL;
    }
    stringstream logMsg;
    logMsg << blockerName <<BLOCKED_MSG << toBlock;
    log(logMsg.str());
    if (NOT userExists(toBlock)) {
        logMsg.clear();
        logMsg.str(ERROR);
        logMsg << toBlock << USER_NOT_EXIST;
        sendMessage(blockerFD, logMsg.str());
        log(logMsg.str());
        return FAIL;
    }
    users.at(toLower(blockerName)).blocked.insert(getFd(toBlock));
    users.at(toLower(toBlock)).followers.erase(blockerFD);

    return SUCCESS;
}

int who(int senderFd) {
    string output;
    map<string,User>::const_iterator it = users.begin();
    for (; it != users.end(); ++it)
    {
        output = it->second.realName;
        if (++it != users.end() )
        {
                output.append("\t");
        }else {
            output.append("\n");
        }
        --it;
        sendMessage (senderFd,output);
    }
    log(getName(senderFd).append(WHO_MSG));
    return 0;
}

int connectClient(int senderFd, const string& name) {
    string lowCaseName = toLower(name);
    stringstream logMsg;

    logMsg << name << CONNECT_MSG;
    log (logMsg.str());
    logMsg.clear();
    logMsg.str(string());
    
    if(fdExist(senderFd)){
        cerr << strip (string(ALREADY_CONNECTED_MSG)) << endl;
        log(ALREADY_CONNECTED_MSG);
        sendMessage(senderFd,ALREADY_CONNECTED_MSG);
        return FAIL; 
    }
    
    if(checkClientName(name.c_str()) < 0){
        cerr << CLIENT_NAME_ILLEGAL_MESSAGE;
        cerr.flush();
        log(CLIENT_NAME_TOO_LONG);
        sendMessage(senderFd,CLIENT_NAME_TOO_LONG);
        close(senderFd);
        FD_CLR (senderFd, &master);
        return FAIL;
    }
      
    if (userExists(lowCaseName)) {
        cerr << CLIENT_EXISTS_MESSAGE;
        cerr.flush();
        logMsg << CLIENT_EXISTS_MESSAGE  ;
        log(logMsg.str());
        sendMessage(senderFd, CLIENT_EXISTS_MESSAGE);
        close(senderFd);
        FD_CLR (senderFd, &master);
        return FAIL;
    }
    usersByFd[senderFd] = lowCaseName;
    users.insert(std::make_pair(lowCaseName, User(name, senderFd)));

 
    sendMessage(senderFd, CONNECTION_SUCCESSFUL_MESSAGE);
    return SUCCESS;
}

bool userExists(const string& userName) {
    string userNameLower = toLower(userName);
    return (users.find(userNameLower) != users.end());
}

int getFd(const string& userName) {
    string lowName = toLower(userName);
    if (NOT userExists(lowName)) {
        return NO_SUCH_USER;
    }
    return users.at(lowName).sockfd;
}

User& getUser(const string& userName) {
    string lowName = toLower(userName);
    return users.at(lowName);
}


int log(const string& message){
    
    
    logFile << message.substr(0,BUFFER_SIZE);
    return logFile.bad();
}


bool fdExist(int fd){
    if ( usersByFd.find(fd) ==  usersByFd.end())
    {
        return false;
    }

    if (!NDEBUG)
    {
    	cerr << userExists(getName(fd)) << endl;
    }
    return userExists(getName(fd));
}



int openLog(){
    logFile.open(LOG_NAME,fstream::out|fstream::trunc);
    return logFile.bad();
}

