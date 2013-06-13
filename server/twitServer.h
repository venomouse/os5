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

const string& getName(int fd) ;
bool connect(int senderFd, const string& name);
bool parseCommand (int senderFd, const string& command);
bool disconnect(const string& sender);
bool twit(const string& sender,const string& message);
bool follow( string& follower,  string& toFollow);
bool unFollow( string& follower,  string& toUnfollow);
bool directMessage(string& sender,string& toAndMessage);
bool block(const string&  blocker,const string&  toBlock);
bool who(const string& sender);
bool log(string message);
string toLower(const string &str);
bool userExists(const string& userNameLowerCase );
bool sendToClient(const string& name ,const string& message );
int getFd(const string& userNameLowerCase);

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

