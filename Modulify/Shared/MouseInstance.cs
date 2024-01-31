using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
    public class MouseInstance
    {
        public string PortName { get; set; }
        public bool IsConnected { get; set; }
        public Profiles CurrentProfile { get; set; }

        public MouseInstance(string portName, bool connected) 
        {
            this.PortName = portName;
            this.IsConnected = connected;
        }
    }
}
