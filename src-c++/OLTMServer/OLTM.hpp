// Class: OLTM
// Purpose: This is the only class that knows about OLTM proper 
//			It contains all the definitions and behaviors implemented in the context of 
//			providing OLTM services
// Naor - 05/14


#ifndef OLTM_H
#define OLTM_H 1

#include "../mycommon/Message.h"
#include "../mycommon/RestMapContainer.h"

#include <string>
using namespace std;

class OLTM
{
public:
	// Commands that will be supported by the OLTM
	static const char* OLTM_COMMAND_UNDEFINED;		// Dummy content
	static const char* OLTM_COMMAND_CREATE;			// create a translation definition in the OLTM 
	static const char* OLTM_COMMAND_READ;			// read a translation definition in the OLTM
	static const char* OLTM_COMMAND_UPDATE;			// update a previous definition with a new one		
	static const char* OLTM_COMMAND_DELETE;			// delete a preview definition already present in the OLTM
	static const char* OLTM_COMMAND_PEEK;			// check fo determine if the OLTM has a particular definition
	static const char* OLTM_COMMAND_SHUTDOWN;		// signal the OLTM server to shutdown itself
	static const char* OLTM_COMMAND_DISCONNECT;		// communicate to the OLTM to close the current socket session

	// Define command shortcuts to be used in the command line client
	static const char* OLTM_CMDLINE_CREATE;
	static const char* OLTM_CMDLINE_READ;
	static const char* OLTM_CMDLINE_UPDATE;
	static const char* OLTM_CMDLINE_DELETE;
	static const char* OLTM_CMDLINE_PEEK;
	static const char* OLTM_CMDLINE_SHUTDOWN;

	// MESSAGE DEFINITIONS
	static const char* OLTM_MESSGE_TYPES[];			// Define array to hold the list of supported messages
	static const char* OLTM_REQUEST_KEYS[];			// Define array for the keys supported in the request message
	static const char* OLTM_RESPONSE_KEYS[];		// Define array for the keys supported in the response message

	// Define the key names supported in the messages
	static const char* OLTM_MESSGE_FIELD_COMMAND;
	static const char* OLTM_MESSGE_FIELD_RESULT;
	static const char* OLTM_MESSGE_FIELD_KEY;
	static const char* OLTM_MESSGE_FIELD_VALUE;

	// Content of special fields
	static const char* OLTM_MESSGE_RESULT_FALSE;
	static const char* OLTM_MESSGE_RESULT_TRUE;

	// Convenience messages
	static const char* OLTM_RESPONSE_RESULT_CREATE;
	static const char* OLTM_RESPONSE_RESULT_DELETE;
	static const char* OLTM_RESPONSE_RESULT_UPDATE;
	static const char* OLTM_RESPONSE_RESULT_PEEK;
	static const char* OLTM_RESPONSE_RESULT_DISCONNECT;
	static const char* OLTM_RESPONSE_RESULT_SHUTDOWN;

private:		// at this point one should only the class - not necessary to have object instances
	OLTM();
	virtual ~OLTM();

public:			// OLTM business methods supported
	static Message	createRequestMessage( const char* command, const char* key, const char *value );
	static Message createResponseMessage( const bool result, const char* value );
	static bool processRequest( Message& request, RestMapContainer& dictionary, string& result );
	static bool processResponse( const string& command, bool retVal, string& result );
	static bool validateRequest( Message& request, string& result );

private:
};

// CLASS VARIABLE INITIALIZATION
// Definitions to be used in the communication of client and server
const char* OLTM::OLTM_COMMAND_UNDEFINED	= "undefined";
const char* OLTM::OLTM_COMMAND_CREATE		= "create";
const char* OLTM::OLTM_COMMAND_READ			= "read";
const char* OLTM::OLTM_COMMAND_UPDATE		= "update";
const char* OLTM::OLTM_COMMAND_DELETE		= "delete";
const char* OLTM::OLTM_COMMAND_PEEK			= "peek";
const char* OLTM::OLTM_COMMAND_SHUTDOWN		= "shutdown";
const char* OLTM::OLTM_COMMAND_DISCONNECT	= "disconnect";

// Shortcut definitions to be used in the cliend command line
const char* OLTM::OLTM_CMDLINE_CREATE		= "-c";
const char* OLTM::OLTM_CMDLINE_READ			= "-r";
const char* OLTM::OLTM_CMDLINE_UPDATE		= "-u";
const char* OLTM::OLTM_CMDLINE_DELETE		= "-d";
const char* OLTM::OLTM_CMDLINE_PEEK			= "-p";
const char* OLTM::OLTM_CMDLINE_SHUTDOWN		= "-s";

// MESSAGE DEFINITIONS
const char* OLTM::OLTM_MESSGE_TYPES[] =  { "request", "response" };
const char* OLTM::OLTM_REQUEST_KEYS[] =  { "command", "key", "value" };
const char* OLTM::OLTM_RESPONSE_KEYS[] = { "result",  "value" };

const char* OLTM::OLTM_MESSGE_FIELD_COMMAND = "command";
const char* OLTM::OLTM_MESSGE_FIELD_RESULT = "result";
const char* OLTM::OLTM_MESSGE_FIELD_KEY = "key";
const char* OLTM::OLTM_MESSGE_FIELD_VALUE = "value";

const char* OLTM::OLTM_MESSGE_RESULT_FALSE = "false";
const char* OLTM::OLTM_MESSGE_RESULT_TRUE = "true";

const char* OLTM::OLTM_RESPONSE_RESULT_CREATE = "Create request successful..";
const char* OLTM::OLTM_RESPONSE_RESULT_DELETE = "Delete request successful...";
const char* OLTM::OLTM_RESPONSE_RESULT_UPDATE = "Update request successful...";
const char* OLTM::OLTM_RESPONSE_RESULT_PEEK = "Found entry for item requested...";
const char* OLTM::OLTM_RESPONSE_RESULT_DISCONNECT = "Ok";
const char* OLTM::OLTM_RESPONSE_RESULT_SHUTDOWN = "Server initiated shutdown...";

// CLASS METHODS IMPLEMENTATION

Message	OLTM::createRequestMessage( const char* command, const char* key, const char *value )
{
	Message msgRet( Message::JSON );

	msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_COMMAND, command );
	msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_KEY, key );
	msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_VALUE, value );

	return msgRet;
}

Message OLTM::createResponseMessage( const bool result, const char* value )
{
	Message msgRet( Message::JSON );

	if( result )
		msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_RESULT, OLTM::OLTM_MESSGE_RESULT_TRUE );
	else
		msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_RESULT, OLTM::OLTM_MESSGE_RESULT_FALSE );

	msgRet.setContent( OLTM::OLTM_MESSGE_FIELD_VALUE, value );
	return msgRet;
}

bool OLTM::processRequest( Message& request, RestMapContainer& dictionary, string& result )
{
	bool bRet = true;
	result = "";
	string command;
	string key;
	string value;

	request.getContent( OLTM::OLTM_MESSGE_FIELD_COMMAND, command );
	request.getContent( OLTM::OLTM_MESSGE_FIELD_KEY, key );

	if( command.compare( OLTM::OLTM_COMMAND_CREATE ) == 0 )
	{
		request.getContent( OLTM::OLTM_MESSGE_FIELD_VALUE, value );

		if( value.size() > 0 )
		{
			if( !dictionary.put( key, value ) )
			{
				result = "Invalid create request - key[";
				result += key;
				result += "] is already present";
				bRet = false;
			}
		}
		else
		{
			result = "Invalid create request - value is empty";
			bRet = false;
		}
	}
	else if( command.compare( OLTM::OLTM_COMMAND_READ ) == 0 )
	{
		if( !dictionary.get( key, result ) )
		{
			result = "Process failed - key[";
			result += key;
			result += "] not present";
			bRet = false;
		}
	}
	else if( command.compare( OLTM::OLTM_COMMAND_UPDATE ) == 0 )
	{
		request.getContent( OLTM::OLTM_MESSGE_FIELD_VALUE, value );

		if( value.size() > 0 )
		{
			if( !dictionary.post( key, value ) )
			{
				result = "Invalid update request - key[";
				result += key;
				result += "] is not present";
				bRet = false;
			}
		}
		else
		{
			result = "Invalid update request - value is empty";
			bRet = false;
		}
	}
	else if( command.compare( OLTM::OLTM_COMMAND_DELETE ) == 0 )
	{
		if( !dictionary.del( key ) )
		{
			result = "Invalid delete request - key[";
			result += key;
			result += "] is not present";
			bRet = false;
		}
	}
	else if( command.compare( OLTM::OLTM_COMMAND_PEEK ) == 0 )
	{
		if( !dictionary.head( key ) )
		{
			result = "Invalid peek request - key[";
			result += key;
			result += "] is not present";
			bRet = false;
		}
	}

	return bRet;
}

bool OLTM::processResponse( const string& command, bool retVal, string& result )
{
	bool bRet = true;

	if( command.compare( OLTM::OLTM_COMMAND_CREATE ) == 0 && retVal )
	{
		result = OLTM::OLTM_RESPONSE_RESULT_CREATE;
	}
	else if( command.compare( OLTM::OLTM_COMMAND_UPDATE ) == 0 && retVal )
	{
		result = OLTM::OLTM_RESPONSE_RESULT_UPDATE;
	}
	else if( command.compare( OLTM::OLTM_COMMAND_DELETE ) == 0 && retVal )
	{
		result = OLTM::OLTM_RESPONSE_RESULT_DELETE;
	}
	else if( command.compare( OLTM::OLTM_COMMAND_PEEK ) == 0 && retVal )
	{
		result = OLTM::OLTM_RESPONSE_RESULT_PEEK;
	}

	return bRet;
}

bool OLTM::validateRequest( Message& request, string& result )
{
	bool bRet = true;
	result = "";
	unsigned size = sizeof(OLTM::OLTM_REQUEST_KEYS)/sizeof( char* );

	for( unsigned count = 0; count < size; ++count )
	{
		if( !request.peekContent( OLTM::OLTM_REQUEST_KEYS[ count ] ) )
		{
			result = "Request invalid - key[";
			result += OLTM::OLTM_REQUEST_KEYS[ count ];
			result += "] not present or error[";
			result += request.getLastMessage();
			result += "]";
			bRet = false;
			break;
		}
	}

	return bRet;
}


#endif