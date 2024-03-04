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
        Task<string> ReceiveData(string sentData);
        void Disconnect();




    }
    public class SerialCommsManager : ISerialCommsManager
    {
        //Object containing SerialPort of Arduino and the profile of the mouse.
        public MouseInstance mouseInstance;
        public event Action OnMouseDetected;
        public event Action OnDataReceived;

        private readonly SemaphoreSlim serialLock = new SemaphoreSlim(1, 1);

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
                await serialLock.WaitAsync();
                var cts = new CancellationTokenSource(1000);
                string response = "";
                //instantiate serial port for each available name
                using (var serialPort = new SerialPort(portName))
                {
                    try
                    {

                        serialPort.BaudRate = 115200;
                        serialPort.DataBits = 8;
                        serialPort.Parity = Parity.None;
                        serialPort.StopBits = StopBits.One;
                        serialPort.DtrEnable = true;
                        serialPort.NewLine = "\n";
                        serialPort.ReadTimeout = 2000;
                        serialPort.DiscardNull = true;
                        serialPort.Handshake = Handshake.None;
                        serialPort.Encoding = System.Text.Encoding.ASCII;

                        //open comms
                        if (!serialPort.IsOpen)
                        {
                            serialPort.Open();
                        }


                        //uses a streamreader object to perform async read/writes, therefore allowing the program to respond while this occurs (Avoids Not Responding)
                        using (var reader = new StreamReader(serialPort.BaseStream))
                        using (var writer = new StreamWriter(serialPort.BaseStream) { AutoFlush = true })
                        {
                            serialPort.DiscardOutBuffer();
                            serialPort.DiscardInBuffer();
                            //sends ident phrase to port
                            await Task.Run(() => serialPort.WriteLine(MouseIDENT));
                            //async reads port response
                            response = serialPort.ReadLine();
                        }
                        Console.WriteLine($"Serial Port Sent Back: {response} in response");
                        //if matches desired response, run ConnectMouse function on the chosen serial port. 

                    }

                    catch (OperationCanceledException ex)
                    {
                        Console.WriteLine($"Serial Port {portName} threw OperationCanceled: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);

                    }
                    catch (UnauthorizedAccessException ex)
                    {
                        //port issues, such as busy
                        Console.WriteLine($"Serial Port {portName} threw UnauthorizedAccess: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);
                    }
                    catch (IOException ex)
                    {
                        //IO errors, such as sudden disconnect, etc
                        Console.WriteLine($"Serial Port {portName} threw IOException: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);
                    }
                    catch (TimeoutException ex)
                    {
                        //if search times out
                        serialPort.Close();
                        serialPort.Open();
                        serialPort.DiscardInBuffer();
                        serialPort.DiscardOutBuffer();
                        serialPort.WriteLine(MouseIDENT);
                        response = serialPort.ReadLine();
                        Console.WriteLine($"Serial Port {portName} threw TimeoutException: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);
                    }
                    catch (ObjectDisposedException ex)
                    {
                        Console.WriteLine($"Serial Port {portName} threw ObjectDisposedException: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"Serial Port {portName} threw Exception: {ex.Message}");
                        Console.WriteLine(ex.StackTrace);
                    }
                    finally
                    {
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
                        serialLock.Release();
                        
                    }
                    return;
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
            mouseInstance.PortName.WriteLine(data);
            await Task.Run(() => Thread.Sleep(500));
            mouseInstance.PortName.WriteLine("<FIN>");
            await Task.Run(() => Disconnect());
            return;
        }

        public async Task<string> ReceiveData(string sentData)
        {
            string dataReceived = "";
            await ConnectMouse(mouseInstance.PortName);
            //asynchronously receives data from mouse. 
            try
            {
                dataReceived = mouseInstance.PortName.ReadLine();
            }
            catch(TimeoutException ex)
            {
                await Task.Run(() => Disconnect());
                mouseInstance.PortName.Open();
                await SendData(sentData);
                dataReceived = mouseInstance.PortName.ReadLine();
            }
            finally
            {
                if (dataReceived != "")
                {
                    OnDataReceived?.Invoke();
                }
                await Task.Run(() => Disconnect());
            }
            

            return dataReceived;
        }

        public async void Disconnect()
        {
            await Task.Run(() => mouseInstance.PortName.Close());
            return;
        }
    }
}
