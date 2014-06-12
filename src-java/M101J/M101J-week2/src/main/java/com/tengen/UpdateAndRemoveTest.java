package com.tengen;


import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

/**
 * Created by naor on 6/9/2014.
 */
public class UpdateAndRemoveTest
{
    public static void main(String[] args) throws UnknownHostException
    {
        DBCollection collection = createCollection();

        List<String> names = Arrays.asList("alice", "bobby", "catthy", "david", "ethan");

        for( String name : names )
        {
            collection.insert(new BasicDBObject("_id",name));
        }

        collection.update(new BasicDBObject("_id","alice"),new BasicDBObject("age",24));
        collection.update(new BasicDBObject("_id","alice"),new BasicDBObject("$set",new BasicDBObject("gender","F")));
        collection.update(new BasicDBObject("_id","frank"),new BasicDBObject("$set",new BasicDBObject("gender","M")),true,false);
        collection.update(new BasicDBObject(),new BasicDBObject("$set",new BasicDBObject("title","Dr.")),false,true);

        collection.remove(new BasicDBObject("_id","alice"));

        printCollection(collection);
    }

    //PRIVATE METHODS
    // these are all the statement I used throughout the lecture.
    private static void scratch(DBCollection collection)
    {
        collection.update(new BasicDBObject("_id", "alice"),
                new BasicDBObject("age", 24));

        collection.update(new BasicDBObject("_id", "alice"),
                new BasicDBObject("$set", new BasicDBObject("age", 24)));

        collection.update(new BasicDBObject("_id", "alice"),
                new BasicDBObject(new BasicDBObject("gender", "F")));

        collection.update(new BasicDBObject("_id", "frank"),
                new BasicDBObject("$set", new BasicDBObject("age", 24)), true, false);

        collection.update(new BasicDBObject(),
                new BasicDBObject("$set", new BasicDBObject("title", "Dr")), false, true);

        collection.remove(new BasicDBObject("_id", "frank"));
    }

    private static DBCollection createCollection() throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB db = client.getDB("course");
        DBCollection collection = db.getCollection("UpdateRemoveTest");
        collection.drop();
        return collection;
    }

    private static void printCollection(final DBCollection collection)
    {
        DBCursor cursor = collection.find().sort(new BasicDBObject("_id", 1));
        try
        {
            while (cursor.hasNext())
            {
                System.out.println(cursor.next());
            }
        } finally {
            cursor.close();
        }
    }

}
