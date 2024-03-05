using Newtonsoft.Json;
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
        [JsonProperty("ProfileName")]
        public string ProfileName { get; set; }
        [JsonProperty("DPI")]
        public string DPI { get; set; }
        [JsonProperty("DPI_SET")]
        public string[] DPI_SET { get; set; }
        [JsonProperty("DEBOUNCE")]
        public string DEBOUNCE { get; set; }

        [Newtonsoft.Json.JsonExtensionData]
        public Dictionary<string, JToken> Keybinds { get; set; }

        public Profiles(string profName, string dpiValue, string[] dpiSet, string debounce, Dictionary<string, JToken> keybinds)
        {
            ProfileName = profName;
            DPI = dpiValue;
            DPI_SET = dpiSet;
            DEBOUNCE = debounce;
            Keybinds = keybinds;
        }

        public Profiles(string jsonFileName) 
        {
            Keybinds = new Dictionary<string, JToken>();

            this.CreateProfileObject(jsonFileName);
        }


        public string StringifyProfile()
        {
            //hardcode the values not stored in dictionary
            string jsonStr =
                "ProfileName:" + ProfileName +
                ";DPI:" + DPI + 
                ";DPI_SET:" + DPI_SET[0] + "+" + 
                              DPI_SET[1] + "+" + 
                              DPI_SET[2] + "+" + 
                              DPI_SET[3] + "+" + 
                              DPI_SET[4] + "+" +
                "DEBOUNCE:" + DEBOUNCE;

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
            string appdataPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            string appFolderPath = Path.Combine(appdataPath, "Modulify");
            string profilePath = Path.Combine(appFolderPath, "Data\\Profiles");
            string jsonPath = Path.Combine(profilePath, $"{jsonTarget}.json");

            //convert stored profile to string
            string jsonStr = "";
            if (File.Exists(jsonPath))
            {
                jsonStr = File.ReadAllText(jsonPath);
            }
            else
            {
                Console.WriteLine("File does not exist");
            }

            //parse into Newtonsoft JSON Object
            JObject jsonObj = JObject.Parse(jsonStr);

            //Manually parse nonbind values into object
            this.ProfileName = jsonObj["ProfileName"].ToString();
            this.DPI = jsonObj["DPI"].ToString();
            this.DPI_SET = jsonObj["DPI_SET"].ToObject<string[]>();
            this.DEBOUNCE = jsonObj["DEBOUNCE"].ToString();

            //pop from jsonobject so that next function can add remaining binds to dictionary
            jsonObj.Remove("ProfileName");
            jsonObj.Remove("DPI");
            jsonObj.Remove("DPI_SET");
            jsonObj.Remove("DEBOUNCE");

            //add keybinds from json file into a dictionary
            this.Keybinds = jsonObj.ToObject<Dictionary<string, JToken>>();  

        }

        public void SaveToJSON()
        {
            string jsonProfile = JsonConvert.SerializeObject(this, Formatting.Indented);
            string appdataPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            string appFolderPath = Path.Combine(appdataPath, "Modulify");
            string profilePath = Path.Combine(appFolderPath, "Data\\Profiles");

            if (!Directory.Exists(profilePath))
            {
                Directory.CreateDirectory(profilePath);
            }
            string filePath = Path.Combine(profilePath, $"{this.ProfileName}.json");

            try
            {
                File.WriteAllText(filePath, jsonProfile);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error occured while saving profile: {ex.Message}");
            }

        }
    }
}
