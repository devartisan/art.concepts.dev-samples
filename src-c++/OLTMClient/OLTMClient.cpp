// OLTMClient.cpp : Naor Toledo Pinto - 05/14
//

#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

#include "../OLTMServer/OLTM.hpp"
#include "../mycommon/Message.h"
#include "../mycommon/RestMapContainer.h"

#include "../mysocket/mySocket.h"
#include "../mysocket/myException.h"
#include "../mysocket/myHostInfo.h"

bool processCall( string& host, string& port, const string& command, string& result, const string& key = string(""), const string& value = string("") );

myLog winLog;

int _tmain(int argc, _TCHAR* argv[])
{
	int iRet = 0;

	if( argc < 4 )
	{
		std::cout << "OLTMClient : utility to test the OLTM Server" << std::endl;
		std::cout << "    usage:" << std::endl;
		std::cout << "           OLTMClient [host] [port] -c [key] [value] -> set the key/value into the OLTM dictionary - Returns [TRUE,FALSE]" << std::endl;
		std::cout << "           OLTMClient [host] [port] -r [key]         -> read value for the given key - Returns [value,<empty string>]" << std::endl;
		std::cout << "           OLTMClient [host] [port] -u [key] [value] -> update value for the given key - Returns [TRUE,FALSE]" << std::endl;
		std::cout << "           OLTMClient [host] [port] -d [key]         -> delete value for the given key - Returns [TRUE,FALSE]" << std::endl;
		std::cout << "           OLTMClient [host] [port] -p [key]         -> check to determine if key/value is present in the OLTM dictionary - Returns [TRUE,FALSE]" << std::endl;
		std::cout << "           OLTMClient [host] [port] -s               -> send signal to server to shutdown itself - Returns [TRUE,FALSE}" << std::endl;

		std::cout << std::endl;
	}
	else
	{
		string host = argv[1];
		string port	= argv[2];
		string commandShortcut = argv[3];
		string command = OLTM::OLTM_COMMAND_UNDEFINED;
		string key = "";
		string value = "";
		string result = "";

		if( commandShortcut.compare( OLTM::OLTM_CMDLINE_SHUTDOWN ) == 0 )
		{
			command = OLTM::OLTM_COMMAND_SHUTDOWN;
		}
		else if( commandShortcut.compare( OLTM::OLTM_CMDLINE_CREATE ) == 0 )
		{
			key = argv[4];
			value = argv[5];

			if( key.size() > 0 && value.size() > 0 )
			{
				command = OLTM::OLTM_COMMAND_CREATE;
			}
			else
			{
				result = "Create command require non empty key[";
				result += key;
				result += "] and value[";
				result += value;
				result += "]";
			}
		}
		else if( commandShortcut.compare( OLTM::OLTM_CMDLINE_READ ) == 0 )
		{
			key = argv[4];

			if( key.size() > 0 )
			{
				command = OLTM::OLTM_COMMAND_READ;
			}
			else
			{
				result = "Read command require non empty key[";
				result += key;
				result += "]";
			}
		}
		else if( commandShortcut.compare( OLTM::OLTM_CMDLINE_UPDATE ) == 0 )
		{
			key = argv[4];
			value = argv[5];

			if( key.size() > 0 && value.size() > 0 )
			{
				command = OLTM::OLTM_COMMAND_UPDATE;
			}
			else
			{
				result = "Update command require non empty key[";
				result += key;
				result += "] and value[";
				result += value;
				result += "]";
			}
		}
		else if( commandShortcut.compare( OLTM::OLTM_CMDLINE_DELETE ) == 0 )
		{
			key = argv[4];

			if( key.size() > 0 )
			{
				command = OLTM::OLTM_COMMAND_DELETE;
			}
			else
			{
				result = "Delete command require non empty key[";
				result += key;
				result += "]";
			}
		}
		else if( commandShortcut.compare( OLTM::OLTM_CMDLINE_PEEK ) == 0 )
		{
			key = argv[4];

			if( key.size() > 0 )
			{
				command = OLTM::OLTM_COMMAND_PEEK;
			}
			else
			{
				result = "Peek command require non empty key[";
				result += key;
				result += "]";
			}
		}
		else
		{
			result = "Parameter command[";
			result += commandShortcut;
			result += "] is not valid ";
		}
		
		if( command.compare(OLTM::OLTM_COMMAND_UNDEFINED) != 0 )
		{
			if( processCall( host, port, command, result, key, value ) )
				std::cout << result << std::endl;
			else
				std::cout << "==> Error:" << result << std::endl;
		}
		else
		{
				std::cout << "==> Error:" << result << std::endl;
		}
	}

	return iRet;
}

bool processCall( string& host, string& port, const string& command, string& result, const string& key, const string& value)
{
	bool bRet = false;

	if( host.size() > 0 )
	{
		int portNum = atoi( port.c_str() );
		if( portNum > 0 )
		{
			// We are ready to make the call to the server
			bRet = true;
			
			// Initialize the winsock library
			WSADATA wsaData;

			try
			{
				if( WSAStartup( 0x101, &wsaData ) )
				{
					myException* initializationException = new myException( 0, "Error: calling WSAStartup()" );
					throw initializationException;
				}
			}
			catch( myException* ex )
			{
				ex->response();
				delete ex;
				bRet = false;
			}

			if( bRet )
			{
				// let's make the call
				myTcpSocket client( portNum );
				client.connectToServer( host, ADDRESS );

				// create message and send it
				Message request = OLTM::createRequestMessage( command.c_str(), key.c_str(), value.c_str() );
				
				string messageContent;

				request.getContent( messageContent );
				client.sendMessage( messageContent );

				// wait for response and process it
				int recvBytes = 0;
				string responseContent = "";
				recvBytes = client.recieveMessage( responseContent );

				if( recvBytes != -99 )
				{
					Message response(Message::JSON);

					response.setContent( responseContent );

					response.getContent( OLTM::OLTM_MESSGE_FIELD_VALUE, result );

					string retVal = "";
					response.getContent( OLTM::OLTM_MESSGE_FIELD_RESULT, retVal );

					if( retVal.compare( OLTM::OLTM_MESSGE_RESULT_TRUE ) == 0 )
					{
						Message disconnect = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_DISCONNECT, "", "" );
						
						string messageContent;

						disconnect.getContent( messageContent );
						client.sendMessage( messageContent );
						
						messageContent = "";
						recvBytes = client.recieveMessage( messageContent );

						if( recvBytes != -99 )
						{
							string ack = "";

							disconnect.setContent( messageContent );
							disconnect.getContent( OLTM::OLTM_MESSGE_FIELD_RESULT, retVal );
							disconnect.getContent( OLTM::OLTM_MESSGE_FIELD_VALUE, ack );

							if( retVal.compare( OLTM::OLTM_MESSGE_RESULT_FALSE ) == 0 &&
								ack.compare( OLTM::OLTM_RESPONSE_RESULT_DISCONNECT ) == 0 )
							{
								OLTM::processResponse( command, true, result );
							}
							else
							{
								result = "Error when receiving disconnect acknowledge[";
								result += ack;
								result += "]";
								bRet = false;
							}
						}
						else
						{
							result = "Error while waiting to receive disconnect acknowledge";
							bRet = false;
						}
					}
					else
					{
						if( command.compare( OLTM::OLTM_COMMAND_SHUTDOWN ) == 0 &&
							result.compare( OLTM::OLTM_RESPONSE_RESULT_SHUTDOWN ) == 0 )
							bRet = true;
						else
							bRet = false;
					}
				}
				else
				{
					result = "Error while waiting to receive response";
					bRet = false;
				}

				// Time to close the winsock library
				try
				{
					if( WSACleanup() )
					{
						myException* cleanupException = new myException( 0, "Error: calling WSACleanup()" );
						throw cleanupException;
					}
				}
				catch( myException* ex )
				{
					ex->response();
					delete ex;
					bRet = false;
				}
			}
		}
		else
		{
			result = "Invalid port number - empty value or [";
			result += port;
			result += "] is not acceptable";
		}
	}
	else
	{
		result = "Invalid host name - empty value is not acceptable";
	}

	return bRet;
}

