var mongodb = require('mongodb');

var mongoClient = mongodb.MongoClient;

// open connection to server
mongoClient.connect('mongodb://localhost:27017/test', 
	function(err,db)
	{
		if(err)
			throw err;
		
		// find one document in our collection
		db.collection('coll').findOne({},
			function(err,doc)
			{
				if(err)
					throw err;
				
				//print result
				console.dir(doc);
				
				//close the db
				db.close();
			});
		
		//declare success
		console.dir("Called findOne");
	});
