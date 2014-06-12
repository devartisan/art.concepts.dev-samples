// Unit Test Implementation for OLTPServer 
// Naor - 05/14

#include "stdafx.h"

#include "../mycommon/Message.h"
#include "../mycommon/RestMapContainer.h"
#include "../OLTMServer/OLTM.hpp"

#ifdef _MSC_VER
#include <boost/config/compiler/visualc.hpp>
#endif

#include <boost/test/minimal.hpp>

bool testMessageObject( Message& message )
{
	bool bRet = true;
	int currentErrorsCount = boost::minimal_test::errors_counter();
	string value;

	BOOST_CHECK( message.setContent( "test message", Message::STRING ));
	BOOST_CHECK( message.getContent( value ));
	BOOST_CHECK( value.compare( "test message" ) == 0); 
	BOOST_CHECK( value.compare( "test message3" ) != 0); 

	BOOST_CHECK( message.setContent( "", Message::STRING ));
	BOOST_CHECK( message.getContent( value ));
	BOOST_CHECK( value.size() == 0); 

	BOOST_CHECK( message.setContent( "", Message::JSON ));
	BOOST_CHECK( message.setContent( "key1", "value1" ));
	BOOST_CHECK( message.getContent( value ));
	BOOST_CHECK( value.size() >= 0); 
	BOOST_CHECK( value.compare( "{\"key1\":\"value2\"}") != 0 ); 

	BOOST_CHECK( message.setContent( "{\"key1\":\"value1\",\"key2\":\"value2\",\"key3\":\"value3\"}", Message::JSON ));
	BOOST_CHECK( message.getContent( value ));
	BOOST_CHECK( value.compare( "{\"key1\":\"value1\",\"key2\":\"value2\",\"key3\":\"value3\"}" ) == 0);
	BOOST_CHECK( message.getContent( "key1", value ));
	BOOST_CHECK( value.compare( "value1" ) == 0); 
	BOOST_CHECK( message.getContent( "key2", value ));
	BOOST_CHECK( value.compare( "value2" ) == 0); 
	BOOST_CHECK( message.getContent( "key3", value ));
	BOOST_CHECK( value.compare( "value3" ) == 0); 

	BOOST_CHECK( message.setContent( "key4", "value4" ));
	BOOST_CHECK( message.getContent( "key4", value ));
	BOOST_CHECK( value.compare( "value4" ) == 0); 

	BOOST_CHECK( message.getContent( value ));
	BOOST_CHECK( value.compare( "{\"key1\":\"value1\",\"key2\":\"value2\",\"key3\":\"value3\",\"key4\":\"value4\"}" ) != 0); 


	if( boost::minimal_test::errors_counter() != currentErrorsCount )
		bRet = false;

	return bRet;
}

bool testRestMapContainer( RestMapContainer& restMap )
{
	bool bRet = true;
	int currentErrorsCount = boost::minimal_test::errors_counter();
	string value;

	BOOST_CHECK( !restMap.post( "key1", "value1" ) );
	BOOST_CHECK( restMap.put( "key1", "value1" ) );
	BOOST_CHECK( restMap.head( "key1" ) );
	BOOST_CHECK( restMap.get( "key1", value ) );
	BOOST_CHECK( value.compare( "value1" ) == 0); 
	BOOST_CHECK( restMap.del( "key1" ) );
	BOOST_CHECK( !restMap.head( "key1" ) );

	restMap.reset();

	if( boost::minimal_test::errors_counter() != currentErrorsCount )
		bRet = false;

	return bRet;
}

bool testOLTMOperations( Message& message, RestMapContainer& restMap )
{
	bool bRet = true;
	int currentErrorsCount = boost::minimal_test::errors_counter();
	string value;
	string result;

	Message msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_CREATE, "key1", "value1" );

	BOOST_CHECK( msg.getContent( OLTM::OLTM_MESSGE_FIELD_COMMAND, value ) );
	BOOST_CHECK( value.compare( "create" ) == 0); 
	BOOST_CHECK( msg.getContent( OLTM::OLTM_MESSGE_FIELD_KEY, value ) );
	BOOST_CHECK( value.compare( "key1" ) == 0); 
	BOOST_CHECK( msg.getContent( OLTM::OLTM_MESSGE_FIELD_VALUE, value ) );
	BOOST_CHECK( value.compare( "value1" ) == 0); 

	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );

	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_READ, "key1", "" );
	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );
	BOOST_CHECK( result.compare( "value1" ) == 0); 

	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_PEEK, "key1", "" );
	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );
	BOOST_CHECK( result.size() == 0); 

	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_UPDATE, "key1", "newValue1" );
	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );

	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_READ, "key1", "" );
	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );
	BOOST_CHECK( result.compare( "newValue1" ) == 0); 

	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_DELETE, "key1", "" );
	BOOST_CHECK( OLTM::processRequest( msg, restMap, result ) );
	msg = OLTM::createRequestMessage( OLTM::OLTM_COMMAND_PEEK, "key1", "" );
	BOOST_CHECK( !OLTM::processRequest( msg, restMap, result ) );

	if( boost::minimal_test::errors_counter() != currentErrorsCount )
		bRet = false;

	return bRet;
}


int test_main( int argc, char* argv[] )
{
	int iRet = 0;
	Message				message;
	RestMapContainer	restMap;
	
	if( !testMessageObject( message ) )
		BOOST_FAIL( "Failed unit testing the Message object" );

	if( !testRestMapContainer( restMap ) )
		BOOST_FAIL( "Failed unit testing the RestMapContainer object" );

	if( !testOLTMOperations( message, restMap ) )
		BOOST_FAIL( "Failed unit testing the OLTM operations" );

	return iRet;
}
