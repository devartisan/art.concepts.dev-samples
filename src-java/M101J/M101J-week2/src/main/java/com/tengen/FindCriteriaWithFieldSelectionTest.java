package com.tengen;

import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.Random;

/**
 * Created by naor on 6/9/2014.
 */
public class FindCriteriaWithFieldSelectionTest
{
    public static void main(String[] args) throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB db = client.getDB("course");
        DBCollection collection = db.getCollection("findCriteriaTest");
        Random rand = new Random();

        collection.drop();

        //insert 10 documents with a random integer as the value of field "x"
        for( int counter=0; counter < 10; counter++ )
        {
            collection.insert( new BasicDBObject("x", rand.nextInt(2))
                                           .append("y", rand.nextInt(100))
                                           .append("z", rand.nextInt(1000)));
        }

        QueryBuilder builder = QueryBuilder.start("x").is(0)
                .and("y").greaterThan(10).lessThan(70);
        DBObject query = new BasicDBObject("x",0)
                                        .append("y",new BasicDBObject("$gt",10).append("$lt",90));

        System.out.println("\nFind all matching entries:");
        // DBCursor it = collection.find(query);
        DBCursor it = collection.find(builder.get(),new BasicDBObject("y",true));

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
    }
}
