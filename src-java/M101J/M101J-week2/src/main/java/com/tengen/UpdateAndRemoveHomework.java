package com.tengen;


import com.mongodb.*;
import org.bson.types.ObjectId;

import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

/**
 * Created by naor on 6/9/2014.
 */
public class UpdateAndRemoveHomework
{
    public static void main(String[] args) throws UnknownHostException
    {
        DBCollection collection = createCollection();

        DBCursor cursor = collection.find(new BasicDBObject("type","homework"))
                                    .sort(new BasicDBObject("student_id", 1).append("score",1));

        Integer lastStudentId = new Integer(-1);
        Integer currStudentId = new Integer(-1);
        while(cursor.hasNext())
        {
            DBObject record = cursor.next();

            currStudentId = (Integer)record.get("student_id");
            String currObjId = record.get("_id").toString();

            if( lastStudentId.intValue() != currStudentId.intValue() )
            {
                WriteResult  result = collection.remove(new BasicDBObject("_id", new ObjectId(currObjId)));
                CommandResult commandResult = result.getLastError();

                if( commandResult.ok() )
                    System.out.println("Remove SUCCESS for student_id=" + currStudentId.toString() + " _id=" + currObjId );
                else
                    System.out.println("Remove ERROR student_id=" + currStudentId.toString() + " _id=" + currObjId + " Error=" + commandResult.getErrorMessage() );

                lastStudentId = currStudentId;
            }
        }
        cursor.close();

        cursor = collection.find(new BasicDBObject("type","homework"))
                .sort(new BasicDBObject("student_id", 1).append("score", 1));

        printCollection(cursor);
    }

    //PRIVATE METHODS
    // these are all the statement I used throughout the lecture.

    private static DBCollection createCollection() throws UnknownHostException
    {
        MongoClient client = new MongoClient();
        DB db = client.getDB("students");
        DBCollection collection = db.getCollection("grades");
        return collection;
    }

    private static void printCollection(final DBCursor cursor)
    {
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
