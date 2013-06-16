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

	return send (socket, message.c_str(), message.size(), 0 );
}

int recvAll (int client_socket, char* buffer)
{
	if (!NDEBUG)
	{
		cerr << "Receiving message" << endl;
	}
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



string getTimeString()
{
    ostringstream ret;
    time_t rawtime;
    struct tm * ptm;
    time ( &rawtime );
    ptm = gmtime ( &rawtime );
    if (ptm->tm_hour < 10) {
        ret << "0";
    }
    ret << ptm->tm_hour <<  ":" ;
        if (ptm->tm_min < 10) {
        ret << "0";
    }
     ret << ptm->tm_min ;
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
    if (spaceLocation < 0 )
    {
    	spaceLocation = stringWithSpace.length() - 1;
    }
    string before = stringWithSpace.substr(0,spaceLocation+1);
    return strip(before);
}

    
string afterSpace(const string& stringWithSpace )
{
    int spaceLocation = stringWithSpace.find_first_of(SPACES);
    if (spaceLocation < 0 )
    {
    	return string ("");
    }
    string after = stringWithSpace.substr(spaceLocation+1,stringWithSpace.size()+1);
    return strip(after);
}




string strip(const string& stringWithSpaces )
{
    int firstSpace = stringWithSpaces.find_first_not_of(SPACES);
    int lastSpace = stringWithSpaces.find_last_not_of(SPACES);
    
    if (firstSpace < 0 || lastSpace <0)
    {
    	return string("");
    }

    return stringWithSpaces.substr(firstSpace,lastSpace+1);
}


string toLower(const string &str)
{
    string newStr = str;
    transform(str.begin(),str.end(), newStr.begin(),::tolower);
    return newStr;
}
