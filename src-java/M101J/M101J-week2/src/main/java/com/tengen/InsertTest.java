package com.tengen;

import com.mongodb.*;
import org.bson.types.ObjectId;

import java.net.UnknownHostException;
import java.util.Arrays;

/**
 * Created by naor on 6/9/2014.
 */
public class InsertTest
{
    public static void main(String[] args) throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB courseDB = client.getDB("course");
        DBCollection collection = courseDB.getCollection("insertTest");

        collection.drop();

        DBObject doc = new BasicDBObject("_id",new ObjectId()).append("x",1);
        DBObject doc2 = new BasicDBObject().append("x",2);

        System.out.println(doc);

        //  collection.insert(doc);
        collection.insert(Arrays.asList(doc,doc2));

        System.out.println(doc);
        System.out.println(doc2);
    }
}
