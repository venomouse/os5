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

#define IS_EQUAL == 0
#define SIGNATURE_LENGTH 4
#define SIGNATURE '%'

#define SERVICE_PREFIX_LENGTH 4
#define MAX_CLIENT_NAME_LENGTH 30
#define BUFFER_SIZE SERVICE_PREFIX_LENGTH + MAX_CLIENT_NAME_LENGTH+MAX_CLIENT_NAME_LENGTH + 140  + sizeof(unsigned int)
#define MAX_MSG_SIZE  BUFFER_SIZE - SERVICE_PREFIX_LENGTH
#define CLIENT_EXISTS_MESSAGE "Error: client name already in use.\n"
#define CLIENT_NAME_ILLEGAL_MESSAGE "Error: illegal client name.\n"
#define CONNECTION_SUCCESSFUL_MESSAGE "Connection successful."
#define SPACES " \n\r\t"
#define ERROR string("Error: ")
#define NDEBUG 1
#define FAIL -1
#define SUCCESS 0

using std::string;

int sendMessage (int socket, std::string message);
int recvAll (int client_socket, char* buffer);

// Get the time from the system according to the exercise format
string getTimeString();

// Creates an error message according to exercise format
string createErrorMsg(const string& userName, const string& errorType);

//Strips the given string from spaces in the beginning and the end of the string
string strip(const string& stringWithSpaces );

//For a string of two parts separated with space/s, returns the part of the string
//before the space
string beforeSpace(const string& stringWithSpace );

//For a string of two parts separated with space/s, returns the part of the string
//after the space
string afterSpace(const string& stringWithSpace );

//Changes the given string to lower case
string toLower(const string &str);

//Changes the give string to upper case
string toUpper(const string &str);


//Checks if the given user name is legal
int checkClientName ( char const* name);


#endif /* COMMUNICATIONFUNCTIONS_H_ */
