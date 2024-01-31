using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class Profiles
    {
        public string Name { get; set; }
        public Dictionary<string, string> Keybinds { get; set; }
        public Profiles(string jsonSerial) 
        {
            Name = jsonSerial.Substring(0,1);
            Keybinds = new Dictionary<string, string>();
        }


        public static Profiles StringToJSON(string jsonSerial)
        {
            Profiles tempProfile = new Profiles(jsonSerial);

            //convert json serial string into profile object

            return tempProfile;

        }

        public string JSONtoString()
        {
            string jsonSerial = "";

            //convert stored profile to string

            return jsonSerial;
        }
    }
}
