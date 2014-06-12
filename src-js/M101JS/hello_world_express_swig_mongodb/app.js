var express = require('express'),
	cons = require('consolidate'),
	mongodb = require('mongodb'),
	app = express(),
	MongoClient = mongodb.MongoClient,
	MongoServer = mongodb.Server;
	
app.engine('html',cons.swig);
app.set('view engine','html');
app.set('views',__dirname + "/views");

var mongoClient = new MongoClient(new MongoServer('localhost',27017,{'native_parser':true}));
var db = mongoClient.db('course');
	
app.get('/',
	function(req,res)
	{
		db.collection('hello_mongo_express').findOne(
			function(err,doc)
			{
				res.render('hello',doc);
			});
	});
	
app.get('*',
	function(req,res)
	{
		res.send("Page not found",404);
	});

mongoClient.open(
	function(err,mongoClient)
	{
		if(err)
			throw err;
		app.listen(8080);
		console.log("Express server started on port 8080");		
	});
