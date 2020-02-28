using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace json
{
    public class Person
    {
        public string Name;
        public int Age;
    }

    class Program
    {
        static void Main(string[] args)
        {
            string outstring;

            outstring = "{%22field1%22:%2200dddww%22,%20%22field2%22:%2222dfddddww%22}";
            
            outstring=outstring.Replace("&22", "\"");
            outstring = outstring.Replace("&20", "\"");

            JavaScriptSerializer serializer = new JavaScriptSerializer();

            Console.Write("Content-Type: text/html\n\n");

            Console.Write(System.Environment.GetEnvironmentVariable("QUERY_STRING"));

            Person Gennry = new Person();
            Gennry = serializer.Deserialize<Person>(outstring);

        }
    }
}
