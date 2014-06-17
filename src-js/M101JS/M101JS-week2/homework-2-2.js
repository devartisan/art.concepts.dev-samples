var MongoClient = require('mongodb').MongoClient;

MongoClient.connect('mongodb://localhost:27017/weather', 
function(err, db) 
{
    if(err) 
		throw err;

    var data = db.collection('data');

    var options = {'sort' : [['State', 1], ['Temperature', -1]] };
    var cursor = data.find({}, {}, options);
	
	var currState = '';

    cursor.each(
	function(err, doc) 
	{
        if(err) 
			throw err;
			
        if(doc != null) 
		{	
			console.dir("State=" + doc.State + " Temperature=" + doc.Temperature + " ID=" + doc._id);

			var state = doc['State'];
			if( currState != state )
			{
				var newDoc = doc;
				currState = state;
				newDoc['month_high'] = true;


				MongoClient.connect('mongodb://localhost:27017/weather', 
				function(err, db) 
				{
					if(err) 
						throw err;

					db.collection('data').save(newDoc,
					function(err,saved)
					{
						if(err)
							throw err;
						
						console.dir("Successfully saved " + saved + " documents for State=" + newDoc.State +  " Temperature=" + newDoc.Temperature);
						return db.close();
					});			
				});
			}
		}
		else
		{
			db.close();
		}
    });
});
