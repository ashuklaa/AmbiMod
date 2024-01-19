using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management;

namespace Modulify.Shared
{
    public class HardwareChangeListener : IDisposable
    {
        private ManagementEventWatcher watcher;

        public HardwareChangeListener()
        {
            var hwQuery = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent");

            watcher = new ManagementEventWatcher(hwQuery);
            watcher.EventArrived += (sender, e) => Task.Run(() => DeviceChangeTriggered(sender, e));
            watcher.Start();
        }

        private void DeviceChangeTriggered(object sender, EventArrivedEventArgs e)
        {
            Console.WriteLine("HW Change Detected");
        }

        public void Dispose()
        {
            watcher.Stop();
            watcher.Dispose();
        }
    }
    public class DeviceManager
    {
        /**
         * Create separate functions for each of the operations requested below. Each function should return the requested information.
         * When a hardware change is registered, scan all connected HID devices until arduino is found.
         * 
         * If arduino is found, then store the name of the port its connected to and return it to the callback location. 
         * 
         * If arduino not found, implement robust error checking 
         * 
         */

        //Hardware Change Event (Link this using WMI API to the OS-level Hardware Changes event (search windows API)
        public event EventHandler HardwareChanged;

        //constructor for device manager, to initialize the Event, portname variable, and other things of that nature that are required. 
        public DeviceManager()
        {

        }

        //async function that monitors hardwarechanged Event, should call OnHardwareChanged using an "Await" call (look at async API for DOTNET C#)
        public async Task MonitorHWChangesStart()
        {


        }

        //async hardware change function that searches all connected ports for arduino
        private async Task OnHardwareChanged(object sender, EventArgs e)
        {

            await SearchForMouse();
        }

        //check each port, if mouse matching vendor and model number of Arduino micro not found, handle exception inside error handling. 
        public async Task SearchForMouse()
        {
            try
            {

            }
            catch (Exception ex)
            {
                await ErrorHandling(ex);
            }

        }

        //Once mouse is found, portname should be passed into this function in order to initialize mouse connection (Use Await for all async functions)
        private async Task OnMouseFound(string portName)
        {

        }

        //handle searching error within this, or if the arduino suddenly disconnects, etc. (any errors you can think of)
        private async Task ErrorHandling(Exception ex)
        {

        }

        //Establish runs every time hardware change function resolves asynchronously. 
        public async void EstablishConnection()
        {
            await SearchForMouse();
        }
    }
}
