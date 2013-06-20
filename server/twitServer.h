/* 
 * File:   twitServer.h
 * Author: yonatan
 *
 * Created on June 10, 2013, 6:27 PM
 */

#ifndef TWITSERVER_H
#define	TWITSERVER_H

#include <set>
#include <vector>
#include <map>
#include <iostream>

using std::string;
using std::set;
using std::map;


int recvAll (int client_socket, char* buffer);


struct User {
    string realName;
    int sockfd;
    set <int> followers;
    set <int> blocked;
    User(string realName,int sockfd) :
        realName(realName),sockfd(sockfd),followers(),blocked(){};
};

map<string,User> users;
map<int,string> usersByFd;
fd_set master; // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()

// Get the name of the client connected to the given socket
string getName(int fd) ;

//Adds the client connected to the socket senderFd to the client data structures,
//using the name parameter
int connectClient(int senderFd, const string& name);

//Parses and executes the given command
int parseCommand (int senderFd, const string& command);

//Disconnects the given user and deletes it from the client data structures
int disconnect(int senderFd);

//Twits the given message as sent the client currently connected to the
// socket senderFd
int twit(int senderFd,const string& message);

//Makes the client connected to senderFd socket follow the client by
// the name of toFollow
int follow( int senderFd,  string& toFollow);

//Makes the client connected to senderFd socket unfollow the client by
// the name of toUnfollow
int unFollow( int senderFd,  string& toUnfollow);

// Sends a direct message from the client connected to the senderFd socket
// to the client embedded in toAndMessage string
int directMessage(int senderFd,string& toAndMessage);

//Makes the client connected to senderFd socket block the client by
// the name of toBlock (the blocker does not get any messages from
//the blocked client
int block(int  blockerFd,const string&  toBlock);

//Sends to the client connected to socket senderFd the list of all
//clients connected currently to the server
int who(int  senderFd);

//Write the given message to the log file
int log(const string& message);

//Checks if the user by the given name (case insensitively) is
//already connected to the server
bool userExists(const string& userName );

//Checks if someone is connected to the socket give by fd
bool fdExist(int fd);                                                                                                                                                                                                                                                                                                                                                                                                                                                   

//Returns the socket associated with the client with the given name
int getFd(const string& userNameLowerCase);

//Opens the log file
int openLog();


// Creates an error message as require by the format of the log
string createErrorMsg(const string& userName, const string& errorType);



#endif	/* TWITSERVER_H */

