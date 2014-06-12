// Class Implementation: RestMapContainer
// Purpose: Implement a Map container that exposes REST operations to the user
// Naor - 05/14

#include "stdafx.h"

#include "RestMapContainer.h"


// Initialization
RestMapContainer::RestMapContainer()
{
}

RestMapContainer::~RestMapContainer()
{
}

// Public Interface
bool RestMapContainer::del( const string& key )
{
	bool bRet = false;
	MapIt it;

	it = this->mapObj.find( key );

	if( it != this->mapObj.end() )
	{
		this->mapObj.erase( it );
		bRet = true;
	}
	
	return bRet;
}

bool RestMapContainer::get( const string& key, string& valRet )
{
	bool bRet = false;
	MapIt it;

	it = this->mapObj.find( key );

	if( it != this->mapObj.end() )
	{
		valRet = it->second;
		bRet = true;
	}

	return bRet;
}

bool RestMapContainer::head( const string& key )
{
	bool bRet = false;
	MapIt it;

	it = this->mapObj.find( key );

	if( it != this->mapObj.end() )
		bRet = true;
	
	return bRet;
}

bool RestMapContainer::post( const string& key, const string& value )
{
	bool bRet = false;
	MapIt it;

	it = this->mapObj.find( key );

	if( it != this->mapObj.end() )
	{
		this->mapObj.erase( it );
		PairRet ret = this->mapObj.insert( PairStrStr( key, value) );

		bRet = ret.second;
	}
	
	return bRet;
}

bool RestMapContainer::put( const string& key, const string& value )
{
	bool bRet = false;
	MapIt it;

	it = this->mapObj.find( key );

	if( it == this->mapObj.end() )
	{
		PairRet ret = this->mapObj.insert( PairStrStr( key, value) );

		bRet = ret.second;
	}
	
	return bRet;
}

// Special services methods
MapKeys RestMapContainer::getKeys()
{
	MapKeys vRet;

	for( MapIt it = this->mapObj.begin(); it != this->mapObj.end(); ++it )
	{
		vRet.push_back( it->first );
	}

	return vRet;
}

void RestMapContainer::reset()
{
	if( !this->mapObj.empty() )
		this->mapObj.clear();
}


// Implementation

