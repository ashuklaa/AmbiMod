

namespace Modulify.Tests
{
    public class ProfileLoadTests
    {

        [Theory]
        [InlineData("profile1")]
        public void Profiles_LoadProfileFromJSON_ProfileLoadTest(string value)
        {
            Profiles tempProfile = new Profiles(value);
            


        }
    }
}