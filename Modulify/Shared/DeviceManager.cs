using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management;
using System.IO.Ports;

namespace Modulify.Shared
{
    public class HardwareChangeListener : IDisposable
    {
        private ManagementEventWatcher watcher;
        private DetectArduino detectArduino;

        public HardwareChangeListener(DetectArduino detectArduino)
        {
            var hwQuery = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent");
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

    public class DetectArduino
    {
        private DeviceManager deviceManager;
        private SettingsManager settingsManager;

        private const string ArduinoIDENT = "Hello";
        private SerialPort _arduinoPort;

        public DetectArduino(DeviceManager deviceManager, SettingsManager settingsManager)
        {
            this.deviceManager = deviceManager;
            this.settingsManager = settingsManager;
        }

        public async Task<string> ReadSerialData(SerialPort serialPort)
        {
            try
            {
                serialPort.ReadTimeout = 1000;
                return serialPort.ReadLine();
            }
            catch(TimeoutException)
            {
                return null;
            }
        }

        public async Task<string> FindArduinoSerial()
        {
            string[] portNames = SerialPort.GetPortNames();
            foreach (var portN in portNames)
            {
                using (SerialPort serialPort = new SerialPort(portN))
                {
                    try
                    {
                        serialPort.BaudRate = 9600;
                        serialPort.Open();

                        string identStr = await ReadSerialData(serialPort);
                        if (identStr.Contains(ArduinoIDENT))
                        {
                            _arduinoPort = serialPort;
                            return portN;
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.ToString());
                    }

                      
                }
            }
            return null;
        }
        public void EstablishConnection(string portName)
        {
            deviceManager.EstablishSerialConnection(portName);
        }
        public async Task LoadSettingsAsync()
        {
            await settingsManager.LoadSettings();
        }
    }
    public class DeviceManager
    {
        /**
         * Create separate functions for each of the operations requested below. Each function should return the requested information.
         * When a hardware change is registered, scan all connected serial devices until arduino is found.
         * 
         * If arduino is found, then store the name of the port its connected to and return it to the callback location. 
         * 
         * If arduino not found, implement robust error checking 
         * 
         */

        static SerialPort _serialPort;

        public DeviceManager()
        {

        }

        public async void EstablishSerialConnection(string portName)
        {
            _serialPort = new SerialPort(portName);

            _serialPort.BaudRate = 9600;

        }



        
    }
}
