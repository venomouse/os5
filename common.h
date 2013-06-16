/*
 * communicationFunctions.h
 *
 *  Created on: Jun 13, 2013
 *      Author: maria
 */

#ifndef COMMUNICATIONFUNCTIONS_H_
#define COMMUNICATIONFUNCTIONS_H_

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <algorithm>
#include <sstream>
#include <iostream>

#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define SERVICE_PREFIX_LENGTH 4
#define MAX_MSG_SIZE  BUFFER_SIZE - SERVICE_PREFIX_LENGTH
#define CLIENT_EXISTS_SERVER_MESSAGE "Error: client name already in use"
#define CONNECTION_SUCCESSFUL_MESSAGE "Connection successful"
#define SPACES " \n\r\t"
#define ERROR string("ERROR ")
#define NDEBUG 0

using std::string;

int sendMessage (int socket, std::string message);
int recvAll (int client_socket, char* buffer);

string getTimeString();
string createErrorMsg(const string& userName, const string& errorType);
string beforeSpace(const string& stringWithSpace );
string afterSpace(const string& stringWithSpace );
string strip(const string& stringWithSpaces );
string& toLower(const string &str);

#endif /* COMMUNICATIONFUNCTIONS_H_ */
