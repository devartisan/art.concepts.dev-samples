var MongoClient = require('mongodb').MongoClient;

MongoClient.connect('mongodb://localhost:27017/course', 
function(err, db) 
{
    if(err) 
		throw err;

    var query = {'student':'Frank','assignment':'hw1'};
    var newDoc = {'student':'Frank','assignment':'hw1','grade':100};
    var options = { 'upsert' : true };

    db.collection('grades').update(query, newDoc, options, 
	function(err, upserted) 
	{
        if(err) 
			throw err;

        console.dir("Successfully upserted " + upserted + " documents!");

        return db.close();
    });
});
