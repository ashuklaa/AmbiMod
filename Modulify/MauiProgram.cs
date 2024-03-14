using Microsoft.Extensions.Logging;
using Modulify.Shared;

namespace Modulify
{
    public static class MauiProgram
    {
        public static MauiApp CreateMauiApp()
        {
            var builder = MauiApp.CreateBuilder();
            builder
                .UseMauiApp<App>()
                .ConfigureFonts(fonts =>
                {
                    fonts.AddFont("Trebuchet-MS.ttf", "Trebuchet-MSRegular");
                });

            builder.Services.AddMauiBlazorWebView();

#if DEBUG
		builder.Services.AddBlazorWebViewDeveloperTools();
		builder.Logging.AddDebug();
#endif

            builder.Services.AddSingleton<ISerialCommsManager, SerialCommsManager>();
            builder.Services.AddTransient<MainPage>();

            return builder.Build();
        }
    }
}