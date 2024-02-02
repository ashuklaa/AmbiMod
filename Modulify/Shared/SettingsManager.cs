using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class SettingsManager
    {
        //constructor to initialize things such as scope variables 
        public SettingsManager() { }
        //load settings from mouse
        public async Task LoadSettings() { }
        //save settings changes to mouse (every time a keybind is changed, call this. 
        public async Task SaveSettings() { }
        //Serialize settingsJSON in order to send it via SaveSettings() to the Arduino
        private string SerializeSettings(object settingsJSON) { return "hello"; }

        //deserialize Arduino Comms into a settingsJSON file, used inside LoadSettings()
        private void DeserializeSettings(object settingsJSON) { }

    }
}
