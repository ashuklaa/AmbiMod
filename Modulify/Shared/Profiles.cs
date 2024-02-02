using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.JavaScript;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class Profiles
    {
        public string ProfileName { get; set; }
        public string DPI { get; set; }
        public Dictionary<string, string> Keybinds { get; set; }

        public Profiles(string jsonFileName) 
        {
            Keybinds = new Dictionary<string, string>();

            this.CreateProfileObject(jsonFileName);
        }


        public string StringifyProfile()
        {
            //hardcode the values not stored in dictionary
            string jsonStr =
                "ProfileName:" + ProfileName +
                ";DPI:" + DPI;

            //run for loop through keybinds dictionary to generate desired string format
            foreach(var bind in Keybinds)
            {
                jsonStr += ";" + bind.Key + ":" + bind.Value;
            }
            //append final semicolon to end of config
            jsonStr += ";";

            //return stringified object, ready to send to arduino. 
            return jsonStr;

        }

        public void CreateProfileObject(string jsonTarget)
        {
            //path to profile json file
            string jsonPath = Environment.CurrentDirectory + "Data/Profiles/" + jsonTarget + ".json";

            //convert stored profile to string
            string jsonStr = File.ReadAllText(jsonPath);

            //parse into Newtonsoft JSON Object
            JObject jsonObj = JObject.Parse(jsonStr);

            //Manually parse nonbind values into object
            this.ProfileName = jsonObj["ProfileName"].ToString();
            this.DPI = jsonObj["DPI"].ToString();

            //pop from jsonobject so that next function can add remaining binds to dictionary
            jsonObj.Remove("ProfileName");
            jsonObj.Remove("DPI");

            //add keybinds from json file into a dictionary
            this.Keybinds = jsonObj.ToObject<Dictionary<string, string>>();  

        }
    }
}
