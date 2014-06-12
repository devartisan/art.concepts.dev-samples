// Class Implementation: Message 
// Purpose: Container of the messages contents as well parsing and packaging of messages
// Naor - 05/14

#include "stdafx.h"

#include "Message.h"

#include <sstream>

#ifdef _MSC_VER
#include <boost/config/compiler/visualc.hpp>
#endif

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Initialization
Message::Message()
{
	this->content = "";
	this->contentType = Message::UNDEFINED;

	setStateFlags( false, false );
	setMessage( "" );
}

Message::Message( messageContentType contentType )
{
	this->content = "";
	this->contentType = contentType;

	setStateFlags( false, false );
	setMessage( "" );
}

Message::~Message()
{
	this->content = "";
	this->contentType = Message::UNDEFINED;
	setStateFlags( false, false );
	setMessage( "" );
}


// Public Interface

bool Message::getContent( string& value )
{
	bool bRet = true;

	if( this->needCompose )
		bRet = compose();

	if( bRet )
		value = this->content;

	return bRet;
}

bool Message::getContent( const string& key, string& value )
{
	bool bRet = true;

	if( this->needParse )
		bRet = parse();

	if( bRet )
		bRet = this->contentMap.get( key, value );

	return bRet;
}

bool Message::getContent( const char* key, string& value )
{
	return getContent( *(new string( key )), value );
}

string Message::getLastMessage()
{
	return this->msg;
}

bool Message::peekContent( const string& key )
{
	bool bRet = true;

	if( this->needParse )
		bRet = parse();
	if( bRet )
		bRet = this->contentMap.head( key );

	return bRet;
}

bool Message::peekContent( const char* key )
{
	return peekContent( *(new string( key )) );
}

bool Message::setContent( const string& key, const string& value )
{
	bool bRet = true;

	if( this->needParse )
		bRet = parse();
	if( bRet )
		bRet = updateContent( key, value );

	return bRet;
}

bool Message::setContent( const char *key, const char *value )
{
	return setContent( *(new string(key)), *(new string(value)) );
}

bool Message::setContent( const string& content, messageContentType contentType )
{
	this->content = content;
	setMessage( "" );

	if( contentType != Message::UNDEFINED && ((this->contentType == Message::UNDEFINED) || (this->contentType != contentType) ) )
		this->contentType = contentType;
		
	if( this->contentType == Message::STRING || this->contentType == Message::UNDEFINED )
		setStateFlags( false, false );
	else
		setStateFlags( true, false );

	return true;
}

bool Message::setContent( const char *content, messageContentType contentType )
{
	return setContent( *(new string( content ) ), contentType );
}


// Implementation
bool Message::compose()
{
	bool bRet = true;

	if( this->contentType == Message::JSON )
		bRet = composeAsJSON();

	if( bRet )
		setComposeFlag( false );

	return bRet;
}

bool Message::composeAsJSON()
{
	bool bRet = true;

	try
	{
		std::ostringstream os;
		boost::property_tree::ptree pt;


		MapKeys keys = this->contentMap.getKeys();

		for( unsigned counter = 0; counter < keys.size(); ++counter )
		{
			string& key = keys.at( counter );
			string value;

			bRet = this->contentMap.get( key, value );

			if( bRet )
				pt.put( key, value );
			else
			{
				string msg = "Error composing JSON content - last composed element had key=[";
				msg += key;
				msg += "]";
				setMessage( msg.c_str() );
				break;
			}
		}

		if( bRet )
		{
			write_json( os, pt, false );
			this->content = os.str();
		}
	}
	catch( std::exception const& ex )
	{
		bRet = false;
		string msg = "Exception composing JSON content - Message=[";
		msg += ex.what();
		msg += "]";
		setMessage( msg.c_str() );
	}

	return bRet;
}

bool Message::parse()
{
	bool bRet = true;

	this->contentMap.reset();

	if( this->contentType == Message::JSON )
		bRet = parseAsJSON();

	if( bRet )
		setParseFlag( false );

	return bRet;
}

bool Message::parseAsJSON()
{
	bool bRet = true;

	if( this->content.size() > 0 )
	{
		try
		{
			std::istringstream is( this->content );
			boost::property_tree::ptree pt;
			
			boost::property_tree::read_json( is, pt );
			boost::property_tree::ptree::const_iterator end = pt.end();

			for( boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it )
			{
				bRet = updateContent( it->first, it->second.get_value<std::string>() );
				
				if( !bRet )
				{
					string msg = "Error parsing JSON content - last parsed element was ";
					msg += it->first;
					msg += "=[";
					msg += it->second.get_value<std::string>();
					msg += "]";
					setMessage( msg.c_str() );
					break;
				}
			}
		}
		catch( std::exception const& ex )
		{
			bRet = false;
			string msg = "Exception parsing JSON content - Message=[";
			msg += ex.what();
			msg += "]";
			setMessage( msg.c_str() );
		}
	}

	return bRet;
}

void Message::setComposeFlag( bool needCompose )
{
	this->needCompose = needCompose;
}

void Message::setMessage( const char* msg )
{
	this->msg = msg;
}

void Message::setParseFlag( bool needParse )
{
	this->needParse = needParse;
}

void Message::setStateFlags( bool needParse, bool needCompose )
{
	this->needParse = needParse;
	this->needCompose = needCompose;
}

bool Message::updateContent( const string& key, const string& value )
{
	if( this->contentMap.head( key ) )
		this->contentMap.post( key, value );
	else
		this->contentMap.put( key, value );
	setComposeFlag( true );

	return true;
}

