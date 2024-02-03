using System.IO.Ports;
using System.Management;

namespace Modulify.Shared
{
    public class SerialCommsManager
    {
        //Object containing SerialPort of Arduino and the profile of the mouse.
        private MouseInstance _mouseInstance;

        //Identification string, send by program as "Marco", awaiting "Polo" response
        private const string MouseIDENT = "Hello";
        private const string ResponseIDENT = "World";

        //default constructor override, calls serial monitoring
        public SerialCommsManager() 
        {
            MonitorSerial();
        }

        //serial monitoring function, creates event watcher for devices connected to system. 
        public void MonitorSerial()
        {
            var hwQuery = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2");
            var watcher = new ManagementEventWatcher(hwQuery);
            
            //If watcher triggers event requested, DetectMouse is run async in the background without awaiting a response (zombied). 
            watcher.EventArrived += async (sender, args) =>
            {
                await DetectMouse().ConfigureAwait(false);
            };
            watcher.Start();
        }
        public async Task DetectMouse()
        {
            //Each port in the serial ports is tested with IDENT string to find arduino (occurs every hardware change)
            foreach(var portName in SerialPort.GetPortNames())
            {
                try
                {
                    //instantiate serial port for each available name
                    using (var serialPort = new SerialPort(portName))
                    {
                        serialPort.BaudRate = 9600;
                        serialPort.NewLine = "\n";

                        //open comms
                        serialPort.Open();
                           
                        //uses a streamreader object to perform async read/writes, therefore allowing the program to respond while this occurs (Avoids Not Responding)
                        using (var reader = new StreamReader(serialPort.BaseStream))
                        using (var writer = new StreamWriter(serialPort.BaseStream))
                        {
                            //flushes write buffer
                            writer.AutoFlush = true;
                            //sends ident phrase to port
                            await writer.WriteLineAsync(MouseIDENT);

                            //async reads port response
                            var response = await reader.ReadLineAsync();

                            //if matches desired response, run ConnectMouse function on the chosen serial port. 
                            if (response.Trim() == ResponseIDENT)
                            {
                                var _ = ConnectMouse(serialPort); //discard variable still runs ConnectMouse async, but doesnt care about response.
                                return;
                            }
                        }

                    }
                }
                catch(UnauthorizedAccessException) 
                {
                    //port issues, such as busy
                }
                catch(IOException)
                {
                    //IO errors, such as sudden disconnect, etc
                }
                catch(TimeoutException) 
                {
                    //if search times out
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Unknown Exception Occured", ex.ToString());
                }
            }
        }
        public async Task ConnectMouse(SerialPort portName)
        {
            //Creates new mouse object with chosen serialport from detectoin function and opens serial communications.
            _mouseInstance = new MouseInstance(portName, true);
            await Task.Run(() => _mouseInstance.PortName.Open());
        }

        public async Task SendData(string data)
        {
            //sends data to mouse asynchronously
            await Task.Run(() => _mouseInstance.PortName.WriteLine(data));
        }

        public async Task<string> ReceiveData()
        {
            string dataReceived = "";

            //asynchronously receives data from mouse. 
            dataReceived = await Task.Run(() => _mouseInstance.PortName.ReadExisting());


            return dataReceived;
        }

        public async void Disconnect()
        {
            await Task.Run(() => _mouseInstance.PortName.Close());
            return;
        }
    }
}
