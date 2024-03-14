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
            //ELLIE: Store dictionary for byte associations for stirng values stored in JSON. 


            //hardcode the values not stored in dictionary
            string jsonStr =
                "PROFILE:" + ProfileName +
                ";DPI:" + DPI +
                ";DPI_SET:" + DPI_SET[0] + "+" +
                              DPI_SET[1] + "+" +
                              DPI_SET[2] + "+" +
                              DPI_SET[3] + "+" +
                              DPI_SET[4] + ";" +
                "DEBOUNCE:" + DEBOUNCE;

            //run for loop through keybinds dictionary to generate desired string format

            //Dictionary that holds numeric index value for keybinds
            //i.e. "MOUSE_4" -> 3
            Dictionary<string, int> keyAssoc = new Dictionary<string, int>()
            {
                { "LEFT_CLICK", 0 },
                { "RIGHT_CLICK", 1 },
                { "MIDDLE_CLICK", 2 },
                { "MOUSE_4", 3 },
                { "MOUSE_5", 4 },
                { "MOUSE_6", 5 },
                { "MOUSE_7", 6 },
                { "MOUSE_8", 7 },
                { "MOUSE_9", 8 },
                { "MOUSE_10", 9 },
                { "MOUSE_11", 10 },
                { "MOUSE_12", 11 }
            };
            Dictionary<string, string> valAssoc = new Dictionary<string, string>()
            {
                //Mouse Keys
                {"MOUSE_LEFT", "1"},
                {"MOUSE_RIGHT", "2"},
                {"MOUSE_MIDDLE", "3"},
                //Keyboard Keys
                {"KEY_LEFT_CTRL", "0x80"},
                {"KEY_LEFT_SHIFT", "0x81"},
                {"KEY_LEFT_ALT", "0x82"},
                {"KEY_LEFT_GUI", "0x83"},
                {"KEY_RIGHT_CTRL", "0x84"},
                {"KEY_RIGHT_SHIFT", "0x85"},
                {"KEY_RIGHT_ALT", "0x86"},
                {"KEY_RIGHT_GUI", "0x87"},
                {"KEY_UP_ARROW", "0xDA"},
                {"KEY_DOWN_ARROW", "0xD9"},
                {"KEY_LEFT_ARROW", "0xD8"},
                {"KEY_RIGHT_ARROW", "0xD7"},
                {"KEY_BACKSPACE", "0xB2"},
                {"KEY_TAB", "0xB3"},
                {"KEY_RETURN", "0xB0"},
                {"KEY_ESC", "0xB1"},
                {"KEY_INSERT", "0xD1"},
                {"KEY_DELETE", "0xD4"},
                {"KEY_PAGE_UP", "0xD3"},
                {"KEY_PAGE_DOWN", "0xD6"},
                {"KEY_HOME", "0xD2"},
                {"KEY_END", "0xD5"},
                {"KEY_CAPS_LOCK", "0xC1"},
                //F Keys
                {"KEY_F1", "0xC2"},
                {"KEY_F2", "0xC3"},
                {"KEY_F3", "0xC4"},
                {"KEY_F4", "0xC5"},
                {"KEY_F5", "0xC6"},
                {"KEY_F6", "0xC7"},
                {"KEY_F7", "0xC8"},
                {"KEY_F8", "0xC9"},
                {"KEY_F9", "0xCA"},
                {"KEY_F10", "0xCB"},
                {"KEY_F11", "0xCC"},
                {"KEY_F12", "0xCD"},
                {"KEY_F13", "0xF0"},
                {"KEY_F14", "0xF1"},
                {"KEY_F15", "0xF2"},
                {"KEY_F16", "0xF3"},
                {"KEY_F17", "0xF4"},
                {"KEY_F18", "0xF5"},
                {"KEY_F19", "0xF6"},
                {"KEY_F20", "0xF7"},
                {"KEY_F21", "0xF8"},
                {"KEY_F22", "0xF9"},
                {"KEY_F23", "0xFA"},
                {"KEY_F24", "0xFB"},

                // Function control keys
                {"KEY_PRINT_SCREEN", "0xCE"},
                {"KEY_SCROLL_LOCK", "0xCF"},
                {"KEY_PAUSE", "0xD0"}
            };
            foreach(var bind in Keybinds)
            {
                string valString = "";
                if (bind.Value.ToString().Contains("+")){
                    string values = (string)bind.Value;
                    var vals = values.Split("+");

                    foreach(var val in vals)
                    {
                        if(valAssoc.TryGetValue(val, out string hexValue))
                        {
                            valString += hexValue + "+";
                        }
                    }
                    valString = valString.TrimEnd('+');
                }
                else
                {
                    if (valAssoc.TryGetValue((string)bind.Value, out string hexValue))
                    {
                        valString = hexValue;
                    }
                    else
                    {
                        valString = "_" + bind.Value.ToString();
                    }
                }
                jsonStr += ";" + keyAssoc[bind.Key] + ":" + valString; //KEY_LEFT_ALT+KEY_RIGHT_ARROW
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
