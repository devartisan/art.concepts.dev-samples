package com.tengen;

import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.Random;

/**
 * Created by naor on 6/9/2014.
 */
public class FindTest
{
    public static void main(String[] args) throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB db = client.getDB("course");
        DBCollection collection = db.getCollection("findTest");

        collection.drop();

        //insert 10 documents with a random integer as the value of field "x"
        for( int counter=0; counter < 10; counter++ )
        {
            collection.insert( new BasicDBObject("x", new Random().nextInt(100)));
        }

        System.out.println("Find one:");
        DBObject doc = collection.findOne();
        System.out.println(doc);

        System.out.println("\nFind all:");
        DBCursor it = collection.find();

        try
        {
            while(it.hasNext())
            {
                DBObject cur = it.next();

                System.out.println(cur);
            }
        }
        finally
        {
            it.close();
        }

        System.out.println("\nCount:");
        long count = collection.count();
        System.out.println(count);
    }
}
