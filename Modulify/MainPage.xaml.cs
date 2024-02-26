using Modulify.Shared;

namespace Modulify
{
    public partial class MainPage : ContentPage
    {
        private readonly ISerialCommsManager _sCommsMgr;

        public MainPage(ISerialCommsManager sCommsMgr)
        {
            InitializeComponent();

            _sCommsMgr = sCommsMgr;
        }
    }
}