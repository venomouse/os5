/*
 * communicationFunctions.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: maria
 */
#include "common.h"

using namespace std;


int sendMessage (int socket, string message)
{
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
	}
	else if (msgSize < 100)
	{
		msgLen = string("0").append(msgLen);
	}

	message = msgLen.append(message);

	return send (socket, message.c_str(), message.size() + SERVICE_PREFIX_LENGTH, 0 );
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



