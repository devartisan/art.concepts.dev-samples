package com.tengen;

import com.mongodb.*;
import freemarker.template.Configuration;
import freemarker.template.Template;
import spark.Request;
import spark.Response;
import spark.Route;
import spark.Spark;

import java.io.StringWriter;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by naor on 6/6/2014.
 */
public class HelloWorldMongoDBSparkFreemarkerStyle
{
    public static void main( String[] args )  throws UnknownHostException
    {
        final Configuration configuration = new Configuration();

        configuration.setClassForTemplateLoading(HelloWorldSparkFreemarkerStyle.class, "/");

        MongoClient client = new MongoClient( new ServerAddress("localhost",27017));

        DB database = client.getDB("course");
        final DBCollection collection = database.getCollection("hello");

        Spark.get(new Route("/") {
            @Override
            public Object handle(final Request request, final Response response) {
                StringWriter writer = new StringWriter();
                try {
                    Template helloTemplate = configuration.getTemplate("hello.ftl");
                    DBObject document = collection.findOne();

                    helloTemplate.process(document, writer);

                    // System.out.println(writer);   // not relevant but could be used as debug
                } catch (Exception ex) {
                    halt(500);
                    ex.printStackTrace();
                }
                return writer;
            }
        });
    }
}
