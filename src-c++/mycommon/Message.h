// Class: Message
// Purpose: Container of the messages contents as well parsing and packaging of messages
// Naor - 05/14


#ifndef message_H
#define message_H 1

#include "RestMapContainer.h"

#include <string>
using namespace std;

class Message
{
public:
   enum messageContentType 
   {
      UNDEFINED,       // Dummy content
      STRING,		 // message type of string - no need for parse or compose
      JSON			 // content of the message is a JSON string
   };

public:
	Message();
	Message( messageContentType contentType );
	virtual ~Message();

	bool	getContent( string& value );
	bool	getContent( const string& key, string& value );
	bool	getContent( const char* key, string& value );
	string	getLastMessage();
	bool	peekContent( const string& key );
	bool	peekContent( const char* key );
	bool	setContent( const string& key, const string& value );
	bool	setContent( const char *key, const char *value );
	bool	setContent( const string& content, messageContentType contentType = Message::UNDEFINED );
	bool	setContent( const char *content, messageContentType contentType = Message::UNDEFINED );

private:
	bool	compose();
	bool	composeAsJSON();
	bool	parse();
	bool	parseAsJSON();
	void	setComposeFlag( bool needCompose );
	void	setMessage( const char* msg );
	void	setParseFlag( bool needParse );
	void	setStateFlags( bool needParse, bool needCompose );
	bool	updateContent( const string& key, const string& value );

private:
	string				content;
	messageContentType	contentType;
	bool				needParse;
	bool				needCompose;
	RestMapContainer	contentMap;
	string				msg;
};

#endif