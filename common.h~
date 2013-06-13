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

#define BUFFER_SIZE 171 + sizeof(unsigned int)
#define SERVICE_PREFIX_LENGTH 4
#define MAX_MSG_SIZE  BUFFER_SIZE - SERVICE_PREFIX_LENGTH
#define CLIENT_EXISTS_SERVER_MESSAGE "Error: client name already in use"

int sendMessage (int socket, std::string message);

int recvAll (int client_socket, char* buffer);

#endif /* COMMUNICATIONFUNCTIONS_H_ */
