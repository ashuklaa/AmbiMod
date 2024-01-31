using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Modulify.Shared
{
public class SerialCommsManager
    {
        private SerialPort _serialPort;
        private const string MouseIDENT = "Hello";
        public void DetectMouse()
        {

        }
        public void ConnectMouse(string portName)
        {

        }

        public void SendData(string data)
        {

        }

        public string ReceiveData()
        {
            string dataReceived = "";

            //implementation

            return dataReceived;
        }

        public void Disconnect()
        {

        }
    }
}
