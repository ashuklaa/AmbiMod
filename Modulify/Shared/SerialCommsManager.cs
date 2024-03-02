using Microsoft.AspNetCore.Authorization.Infrastructure;
using System.IO.Ports;
using System.Management;
using Windows.Networking;

namespace Modulify.Shared
{
    public interface ISerialCommsManager
    {
        event Action OnMouseDetected;
        event Action OnDataReceived;
        MouseInstance MouseInstance { get; }
        Task DetectMouse();
        bool MouseFound();
        Task ConnectMouse(SerialPort portName);
        Task SendData(string data);
        Task<string> ReceiveData();
        void Disconnect();




    }
    public class SerialCommsManager : ISerialCommsManager
    {
        //Object containing SerialPort of Arduino and the profile of the mouse.
        public MouseInstance mouseInstance;
        public event Action OnMouseDetected;
        public event Action OnDataReceived;

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
                        serialPort.DataBits = 8;
                        serialPort.Parity = Parity.None;
                        serialPort.StopBits = StopBits.One;
                        serialPort.DtrEnable = true;
                        serialPort.NewLine = "\n";
                        serialPort.ReadTimeout = 1000;
                        serialPort.DiscardNull = true;
                        string response = "";
                        //open comms

                        if (serialPort.IsOpen)
                        {
                            await Task.Run(() => serialPort.Close());
                            Thread.Sleep(500);
                        }
                        serialPort.Open();
                        await Task.Run(() => Thread.Sleep(3000));

                        ////uses a streamreader object to perform async read/writes, therefore allowing the program to respond while this occurs (Avoids Not Responding)
                        using (var reader = new StreamReader(serialPort.BaseStream))
                        using (var writer = new StreamWriter(serialPort.BaseStream) { AutoFlush = true })
                        {
                            serialPort.DiscardInBuffer(); 
                            serialPort.DiscardOutBuffer();
                            //sends ident phrase to port
                            await writer.WriteLineAsync(MouseIDENT);
                            //async reads port response
                            response = await reader.ReadLineAsync();
                            Console.WriteLine($"Serial Port Sent Back: {response} in response");
                        }
                        //if matches desired response, run ConnectMouse function on the chosen serial port. 
                        if (response.Trim() == ResponseIDENT)
                        {
                            
                            if (!MouseFound())
                            {
                                mouseInstance = new MouseInstance(serialPort, true);
                            }
                            OnMouseDetected?.Invoke();
                            //await ConnectMouse(serialPort);

                        }

                        await Task.Run(() => serialPort.Close());
                        Console.WriteLine($"Serial Port closed on {portName}");
                        return;
                    }
                }
                catch(UnauthorizedAccessException ex) 
                {
                    //port issues, such as busy
                    Console.WriteLine($"Serial Port {portName} threw UnauthorizedAccess: {ex.Message}");
                    Console.WriteLine(ex.StackTrace);
                }
                catch(IOException ex)
                {
                    //IO errors, such as sudden disconnect, etc
                    Console.WriteLine($"Serial Port {portName} threw IOException: {ex.Message}");
                    Console.WriteLine(ex.StackTrace);
                }
                catch(TimeoutException ex) 
                {
                    //if search times out
                    Console.WriteLine($"Serial Port {portName} threw TimeoutException: {ex.Message}");
                    Console.WriteLine(ex.StackTrace);
                }
                catch(ObjectDisposedException ex)
                {
                    Console.WriteLine($"Serial Port {portName} threw ObjectDisposedException: {ex.Message}");
                    Console.WriteLine(ex.StackTrace);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Serial Port {portName} threw Exception: {ex.Message}");
                    Console.WriteLine(ex.StackTrace);
                }
            }
        }

        public bool MouseFound()
        {
            return mouseInstance?.IsConnected ?? false;
        }
        public async Task ConnectMouse(SerialPort portName)
        {
            //Creates new mouse object with chosen serialport from detection function and opens serial communications.
            //mouseInstance = new MouseInstance(portName, true);
            if(mouseInstance == null) 
            {
                mouseInstance = new MouseInstance(portName, true);
            }
            if (mouseInstance.PortName.IsOpen)
            {
                await Task.Run(() => mouseInstance.PortName.Close());
            }
            await Task.Run(() => mouseInstance.PortName.Open());
            OnMouseDetected?.Invoke();
        }

        public async Task SendData(string data)
        {
            await ConnectMouse(mouseInstance.PortName);
            //sends data to mouse asynchronously
            using (var writer = new StreamWriter(mouseInstance.PortName.BaseStream))
            {
                writer.AutoFlush = true;

                await writer.WriteLineAsync(data);
            }
            return;
        }

        public async Task<string> ReceiveData()
        {
            string dataReceived = "";
            await ConnectMouse(mouseInstance.PortName);
            //asynchronously receives data from mouse. 
            using (var reader = new StreamReader(mouseInstance.PortName.BaseStream))
            {
                dataReceived = await reader.ReadLineAsync();
                OnDataReceived?.Invoke();
            };

            await Task.Run(() => Disconnect());

            return dataReceived;
        }

        public async void Disconnect()
        {
            await Task.Run(() => mouseInstance.PortName.Close());
            return;
        }
    }
}
