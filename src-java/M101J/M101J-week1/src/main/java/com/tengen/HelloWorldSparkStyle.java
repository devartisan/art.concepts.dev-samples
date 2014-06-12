package com.tengen;

import spark.Request;
import spark.Response;
import spark.Route;
import spark.Spark;

/**
 * Created by naor on 6/5/2014.
 */
public class HelloWorldSparkStyle
{
    public static void main( String[] args )
    {
        Spark.get(new Route("/")
        {
            @Override
            public Object handle(final Request request, final Response response)
            {
                return "Hello World From Spark";
            }
        });
    }
}
