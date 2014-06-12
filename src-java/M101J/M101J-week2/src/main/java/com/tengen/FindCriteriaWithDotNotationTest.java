package com.tengen;



import com.mongodb.*;

import java.net.UnknownHostException;
import java.util.Random;

/**
 * Created by naor on 6/9/2014.
 */
public class FindCriteriaWithDotNotationTest
{
    public static void main(String[] args) throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB db = client.getDB("course");
        DBCollection lines = db.getCollection("DotNotationTest");
        Random rand = new Random();

        lines.drop();

        //insert 10 documents with a random integer as the value of field "x"
        for( int counter=0; counter < 10; counter++ )
        {
            lines.insert(new BasicDBObject("_id", counter)
                    .append("start", new BasicDBObject("x", rand.nextInt(90) + 10 ).append("y", rand.nextInt(90) + 10))
                    .append("end",  new BasicDBObject("x", rand.nextInt(90) + 10 ).append("y", rand.nextInt(90) + 10)));
        }

        QueryBuilder builder = QueryBuilder.start("start.x").greaterThan(50);

        System.out.println("\nFind all matching entries:");
        DBCursor it = lines.find(builder.get(),new BasicDBObject("start.y", true).append("_id",false));

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
