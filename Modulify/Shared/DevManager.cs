using Microsoft.Maui.ApplicationModel.DataTransfer;
using Microsoft.Maui.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class DevManager : IDisposable
    {
        private ManagementEventWatcher _watcher;
        private ManagementEventWatcher _disconnectWatcher;
        private MouseInstance _arduinoMouse;

        public DevManager(MouseInstance arduinoMouse)
        {
            _arduinoMouse = arduinoMouse;
        }

        public void StartDeviceMonitoring()
        {
            string connectQuery = "SELECT* FROM __InstanceCreationEvent WITHIN 2 WHERE TargetInstance ISA 'Win32_PnPEntity'";
            var hwQuery = new WqlEventQuery(connectQuery);
            _watcher = new ManagementEventWatcher(hwQuery);
            _watcher.EventArrived += OnDeviceChangeEvent;
            _watcher.Start();
        }

        public void OnDeviceChangeEvent(object sender, EventArrivedEventArgs e)
        {
            var instance = (ManagementBaseObject)e.NewEvent["TargetInstance"];
            string deviceID = instance["DeviceID"].ToString();

            if (instance["Description"].ToString().Contains("Arduino Micro"))
            {
                var portSearcher = new ManagementObjectSearcher($"SELECT * FROM Win32_SerialPort WHERE PNPDeviceID = '{deviceID}'");

                var obj = portSearcher.Get().OfType<ManagementObject>().FirstOrDefault();

                if (obj != null)
                {
                    var portName = obj["DeviceID"].ToString();
                    _arduinoMouse = new MouseInstance(portName, true);
                }
                else
                {
                    Console.WriteLine("No mouse detected");
                }
            }
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }

        public event EventHandler DeviceConnected;
        public event EventHandler DeviceDisconnected;
    }

    public class HWChangeListener : IDisposable
    {
        private ManagementEventWatcher watcher;
        private DetectArduino detectArduino;

        public HWChangeListener(DetectArduino detectArduino)
        {
            string connectQuery = "SELECT * FROM Win32_PnPEntity WHERE Description LIKE '%Arduino%'";
            var hwQuery = new WqlEventQuery(connectQuery);
            this.detectArduino = detectArduino;

            watcher = new ManagementEventWatcher(hwQuery);
            watcher.EventArrived += (sender, e) => Task.Run(() => DeviceChangeTriggered(sender, e));
            watcher.Start();
        }

        private async void DeviceChangeTriggered(object sender, EventArrivedEventArgs e)
        {
            Console.WriteLine("HW Change Detected");
            string arduinoPort = await detectArduino.FindArduinoSerial();

            if (!string.IsNullOrEmpty(arduinoPort))
            {
                detectArduino.Equals(arduinoPort);
                await detectArduino.LoadSettingsAsync();
            }
        }

        public void Dispose()
        {
            watcher.Stop();
            watcher.Dispose();
        }
    }
}
