/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package devartisan;

import java.io.InputStream;
import net.sf.json.JSON;
import net.sf.json.JSONSerializer;
import net.sf.json.xml.XMLSerializer;
import org.apache.commons.io.IOUtils;


/**
 *
 * @author naor
 */
public class ConvertJSONToXML
{
    public static void main(String[] args) throws Exception
    {
        String jsonData = "{'foo':'bar','coolness':2.0,'altitude':39000,'pilot':{'firstName':'Buzz','lastName':'Aldrin'},'mission':'apollo 11'}";

        InputStream is = ConvertJSONToXML.class.getResourceAsStream("sample-json.txt");
        
        if( is != null )
        {
            jsonData = IOUtils.toString(is);

        }
        
        XMLSerializer serializer = new XMLSerializer();
        JSON json = JSONSerializer.toJSON(jsonData);
        String xml = serializer.write(json);
        System.out.println(xml);
    }
}
 