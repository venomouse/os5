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


using namespace std;
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

string getName(int fd) ;
int connectClient(int senderFd, const string& name);
int parseCommand (int senderFd, const string& command);
int disconnect(int senderFd);
int twit(int senderFd,const string& message);
int follow( int senderFd,  string& toFollow);
int unFollow( int senderFd,  string& toUnfollow);
int directMessage(int senderFd,string& toAndMessage);
int block(int  blockerFd,const string&  toBlock);                                                                                                                                                                                                  
int who(int  senderFd);                                                 
int log(const string& message);
bool userExists(const string& userNameLowerCase );
bool fdExist(int fd);                                                                                                                                                                                                                                                                                                                                                                                                                                                   
int getFd(const string& userNameLowerCase);
int openLog();

string strip(const string& stringWithSpaces );
string beforeSpace(const string& stringWithSpace );
string afterSpace(const string& stringWithSpace );


string createErrorMsg(const string& userName, const string& errorType);
string getTimeString();
/*
 
 * 
 
 * twit (const string& sender, const string& twit)
 * disconnect(string username);
 
 * who()
 * log(string message);
 
 
 */


#endif	/* TWITSERVER_H */

