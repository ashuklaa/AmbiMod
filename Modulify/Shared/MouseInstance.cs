using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class MouseInstance
    {
        public SerialPort PortName { get; set; }
        public bool IsConnected { get; set; }
        public Profiles CurrentProfile { get; set; }

        public MouseInstance(SerialPort portName, bool connected, string activeProfile) 
        {
            this.PortName = portName;
            this.IsConnected = connected;
            this.CurrentProfile = new Profiles(activeProfile);
            //Console.WriteLine($"Arduino successfully detected on port {portName.ToString()}. Connected: {connected}");
        }
    }
}
