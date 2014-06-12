// Class: RestMapContainer
// Purpose: Implement a Map container that exposes REST operations to the user
// Naor - 05/14


#ifndef RestMapContainer_H
#define RestMapContainer_H 1

#include <map>
#include <string>
#include <vector>
using namespace std;

typedef std::map<std::string, std::string> MapStrStr;
typedef std::pair<std::string, std::string> PairStrStr;
typedef MapStrStr::iterator MapIt;
typedef std::pair<MapIt, bool> PairRet;
typedef std::vector<std::string> MapKeys;

class RestMapContainer
{
public:
	RestMapContainer();
	virtual ~RestMapContainer();


	bool del( const string& key );
	bool get( const string& key, string& valRet );
	bool head( const string& key );   // only to determine if we have an entry for the key in the map
	bool post( const string& key, const string& value );
	bool put( const string& key, const string& value );

	// Convenience (special services) methods
	MapKeys getKeys();			// get access to all keys in the map for other calls
	void reset();				// clean current map

private:

private:
   MapStrStr mapObj;
};

#endif