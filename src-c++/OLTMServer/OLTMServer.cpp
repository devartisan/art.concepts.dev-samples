// OLTMServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>

#include <iostream>
#include <string>

#include "../mysocket/mySocket.h"
#include "../mysocket/myLog.h"
#include "../mysocket/myException.h"
#include "../mysocket/myHostInfo.h"

#include "../mycommon/Message.h"
#include "../mycommon/RestMapContainer.h"

#include "OLTM.hpp"

myLog winLog;

int _tmain(int argc, _TCHAR* argv[])
{
	int iRet = 0;
	int iPort = PORTNUM;
	RestMapContainer dictionaryObj;
	bool stayAlive = true;

	if( argc == 1 )
	{
		std::cout << "OLTMServer : proof of concept for OLTM Server" << std::endl;
		std::cout << "    usage:" << std::endl;
		std::cout << "           OLTMServer [port] -> set the port the server will use for listen to client requests" << std::endl;

		std::cout << std::endl;
	}

	if( argc > 1 )
	{
		// Lets parse the port number we will be working with
		iPort = atoi( argv[1] );

		// Initialize the winsock library
		WSADATA wsaData;

		winLog << "system started ..." << std::endl;

		winLog << "initialize the socket library..." << std::endl;

		try
		{
			if( WSAStartup( 0x101, &wsaData ) )
			{
				myException *initializationException = new myException( 0, "Error: calling WSAStartup()" );
				throw initializationException;
			}
		}
		catch( myException *ex )
		{
			ex->response();
			delete ex;
			iRet = -1;
		}
		winLog << "end of socket library initialization..." << std::endl;

		if( iRet == 0 )
		{
			winLog << "Retrieve the local host and address:" << std::endl;

			myHostInfo thisHost;
			string thisHostName = thisHost.getHostName();
			string thisHostAddr = thisHost.getHostIPAddress();

			std::cout << "------------------------------------------------------------" << std::endl;
			std::cout << "  Local Host Information" << std::endl;
			std::cout << "   Machine Name: " << thisHostName << std::endl;
			std::cout << "   IP Address:   " << thisHostAddr << std::endl;
			std::cout << "------------------------------------------------------------" << std::endl;

			winLog << "Machine Name:" << thisHostName << std::endl;
			winLog << "IP Address:" << thisHostAddr << std::endl;

			myTcpSocket thisServer( iPort );
			std::cout << thisServer;
			winLog << "This Server Configuration" << std::endl;
			winLog << thisServer;

			thisServer.bindSocket();
			std::cout << "server finished binding process..." << std::endl;
			winLog << "server finished binding process ..." << std::endl;

			thisServer.listenToClient();
			std::cout << "server is waiting for client connections..." << std::endl;
			winLog << "server is waiting for client connections..." << std::endl;
	
			// Server lifecycle loop
			while( stayAlive )
			{
				myTcpSocket* client;
				string clientHost;
				bool inSession = true;
				
				client = thisServer.acceptClient( clientHost );
				winLog << "==> Client [" << clientHost << "] is connected" << std::endl;

				// Request (session) lifecycle loop 
				while( inSession )
				{
					string clientMessageIn = "";
					string resultValue = "";
					string command = "";

					// receive from client
					int numBytes = client->recieveMessage( clientMessageIn );

					if( numBytes != -99 )
					{
						Message requestMsg;

						requestMsg.setContent( clientMessageIn, Message::JSON );

						if( OLTM::validateRequest( requestMsg, resultValue ) )
						{
							requestMsg.getContent( OLTM::OLTM_MESSGE_FIELD_COMMAND, command );

							if( command.compare( OLTM::OLTM_COMMAND_DISCONNECT) == 0 )
							{
								Message response = OLTM::createResponseMessage( false, OLTM::OLTM_RESPONSE_RESULT_DISCONNECT );
								string msg;

								response.getContent( msg );
								client->sendMessage( msg );

								inSession = false;
							}
							else if( command.compare( OLTM::OLTM_COMMAND_SHUTDOWN) == 0 )
							{
								Message response = OLTM::createResponseMessage( false, OLTM::OLTM_RESPONSE_RESULT_SHUTDOWN );
								string msg;

								response.getContent( msg );
								client->sendMessage( msg );

								inSession = false;
								stayAlive = false;
							}
							else
							{
								// This is the core of the server process
								// Here is the place the system will touch the actual repository of 
								// translation definitions
								if( OLTM::processRequest( requestMsg, dictionaryObj, resultValue ) )
								{
									Message response = OLTM::createResponseMessage( true, resultValue.c_str() );
									string msg;

									response.getContent( msg );
									client->sendMessage( msg );
								}
								else
								{
									Message response = OLTM::createResponseMessage( false, resultValue.c_str() );
									string msg;

									response.getContent( msg );

									client->sendMessage( msg ); 
									winLog << "error ==>" << resultValue << std::endl;
									inSession = false;
								}
							}
						}
						else
						{
							Message response = OLTM::createResponseMessage( false, resultValue.c_str() );
							string msg;

							response.getContent( msg );

							client->sendMessage( msg ); 
							winLog << "error ==>" << resultValue << std::endl;
							inSession = false;
						}
					}
					else
					{
						resultValue = "Error reading message from [";
						resultValue += clientHost;
						resultValue += "]";
						winLog << "==>" << resultValue << std::endl;
						inSession = false;
					}
				}
			}
		}
		winLog << "server about to shutdown..." << std::endl;
		// Close the winsock Library
#ifdef WINDOWS_XP
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
			iRet = -1;
		}
#endif
		winLog << "shutdown successful - good bye!!!" << std::endl;
	}

	return iRet;
}

