package com.tengen;

import freemarker.template.Configuration;
import freemarker.template.Template;
import spark.Request;
import spark.Response;
import spark.Route;
import spark.Spark;

import java.io.StringWriter;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by naor on 6/6/2014.
 */
public class HelloWorldSparkFreemarkerStyle
{
    public static void main( String[] args )
    {
        final Configuration configuration = new Configuration();

        configuration.setClassForTemplateLoading(HelloWorldSparkFreemarkerStyle.class, "/");

        Spark.get(new Route("/") {
            @Override
            public Object handle(final Request request, final Response response)
            {
                StringWriter writer = new StringWriter();
                try
                {
                    Template helloTemplate = configuration.getTemplate("hello.ftl");
                    Map<String,Object> helloMap = new HashMap<String,Object>();

                    helloMap.put("name","Freemarker");
                    helloTemplate.process(helloMap,writer);

                    // System.out.println(writer);   // not relevant but could be used as debug
                }
                catch( Exception ex )
                {
                    halt(500);
                    ex.printStackTrace();
                }
                return writer;
            }
        });
    }
}
