using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class ConfigManager
    {
        public Profiles LoadProfile(string profileName)
        {
            Profiles tempProfile = new Profiles(profileName);
            return tempProfile;
        }
        public void SaveProfile(Profiles profile)
        {

        }

        public void UpdateProfile(string profileName, Profiles profile)
        {

        }

        public void SendProfile(Profiles profile) { }

    }
}
