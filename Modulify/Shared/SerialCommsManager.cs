using System.IO.Ports;
using System.Management;
using Windows.Networking;

namespace Modulify.Shared
{
    public interface ISerialCommsManager
    {
        event Action OnMouseConnected;
        MouseInstance MouseInstance { get; }
        Task DetectMouse();
        bool MouseFound();
        Task ConnectMouse(SerialPort portName);
        Task SendData(string data);
        Task<string> ReceiveData();




    }
    public class SerialCommsManager : ISerialCommsManager
    {
        //Object containing SerialPort of Arduino and the profile of the mouse.
        public MouseInstance mouseInstance;
        public event Action OnMouseConnected;

        //Identification string, send by program as "Marco", awaiting "Polo" response
        private const string MouseIDENT = "Hello";
        private const string ResponseIDENT = "World";

        public MouseInstance MouseInstance
        {
            get { return mouseInstance; }
        }


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
                await DetectMouse();
            };
            watcher.Start();
        }
        public async Task DetectMouse()
        {
            if (MouseFound())
            {
                OnMouseConnected?.Invoke();
                return;
            }
            Console.WriteLine($"List of Portnames: {SerialPort.GetPortNames()}");
            //Each port in the serial ports is tested with IDENT string to find arduino (occurs every hardware change)
            foreach (var portName in SerialPort.GetPortNames())
            {
                try
                {
                    //instantiate serial port for each available name
                    using (var serialPort = new SerialPort(portName))
                    {
                        serialPort.BaudRate = 9600;
                        serialPort.NewLine = "\n";
                        serialPort.DtrEnable = true;
                        serialPort.RtsEnable = true;

                        //open comms
                        serialPort.Open();
                        Console.WriteLine($"Serial Port opened on {portName}");
                           
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
                            Console.WriteLine($"Serial Port Sent Back: {response} in response");

                            //if matches desired response, run ConnectMouse function on the chosen serial port. 
                            if (response.Trim() == ResponseIDENT)
                            {
                                Console.WriteLine("Handshake Successful");
                                serialPort.Close();
                                
                                await ConnectMouse(serialPort); //discard variable "_" still runs ConnectMouse async, but doesnt care about response.
                                
                                return;
                            }
                        }
                        //close comms on port if response is not as expected
                        serialPort.Close();
                        Console.WriteLine($"Serial Port closed on {portName}");

                    }
                }
                catch(UnauthorizedAccessException ex) 
                {
                    //port issues, such as busy
                    Console.WriteLine($"Serial Port {portName} threw UnauthorizedAccess: {ex.Message}");
                }
                catch(IOException ex)
                {
                    //IO errors, such as sudden disconnect, etc
                    Console.WriteLine($"Serial Port {portName} threw IOException: {ex.Message}");
                }
                catch(TimeoutException ex) 
                {
                    //if search times out
                    Console.WriteLine($"Serial Port {portName} threw TimeoutException: {ex.Message}");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Serial Port {portName} threw Exception: {ex.Message}");
                }
            }
        }

        public bool MouseFound()
        {
            return mouseInstance?.IsConnected ?? false;
        }
        public async Task ConnectMouse(SerialPort portName)
        {
            //Creates new mouse object with chosen serialport from detectoin function and opens serial communications.
            mouseInstance = new MouseInstance(portName, true);
            
            await Task.Run(() => mouseInstance.PortName.Open());
            OnMouseConnected?.Invoke();
        }

        public async Task SendData(string data)
        {
            //sends data to mouse asynchronously
            await Task.Run(() => mouseInstance.PortName.WriteLine(data));
        }

        public async Task<string> ReceiveData()
        {
            string dataReceived = "";

            //asynchronously receives data from mouse. 
            dataReceived = await Task.Run(() => mouseInstance.PortName.ReadExisting());


            return dataReceived;
        }

        public async void Disconnect()
        {
            await Task.Run(() => mouseInstance.PortName.Close());
            return;
        }
    }
}
