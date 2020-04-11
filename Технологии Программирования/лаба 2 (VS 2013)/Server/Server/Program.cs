using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace Server
{
    class Program
    {
        static int port = 8005; // порт для приема входящих запросов
        const string defaultPath = "C:\\";

        private static string GetAllInDir(string path)
        {
            string result = "dir|" + path;
            DirectoryInfo dir = new DirectoryInfo(path);

            if (path != defaultPath)
                result += "|..";

            //запись всех папок в текущей папке
            foreach (var item in dir.GetDirectories())
            {
                result += "|" + item.Name;
            }

            //запись всех файлов в текущей папке
            foreach (var item in dir.GetFiles())
            {
                result += "|" + item.Name;
            }

            return result;
        }

        private static string GetFile(string path)
        {
            string data = "txt|" + path + "|";

            FileStream file = File.OpenRead(path);

            // преобразуем строку в байты
            byte[] array = new byte[file.Length];
            // считываем данные
            file.Read(array, 0, array.Length);
            // декодируем байты в строку
            data += System.Text.Encoding.ASCII.GetString(array);

            file.Close();

            return data;
        }

        private static void SendData(string message, Socket handler, string consoleText)
        {
            byte[] data = new byte[256];

            Console.WriteLine(DateTime.Now.ToShortTimeString() + ": " + consoleText);
            data = Encoding.Unicode.GetBytes(message);
            handler.Send(data);
        }

        static void Main(string[] args)
        {
            // получаем адреса для запуска сокета
            IPEndPoint ipPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), port);

            // создаем сокет
            Socket listenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                // связываем сокет с локальной точкой, по которой будем принимать данные
                listenSocket.Bind(ipPoint);

                // начинаем прослушивание
                listenSocket.Listen(10);

                Console.WriteLine("Сервер запущен. Ожидание подключений...");

                while (true)
                {
                    string directory = defaultPath;
                    Socket handler = listenSocket.Accept();
                    // получаем сообщение

                    bool isConnected = true;
                    while (isConnected)
                    {
                        StringBuilder builder = new StringBuilder();
                        int bytes = 0; // количество полученных байтов
                        byte[] data = new byte[256]; // буфер для получаемых данных

                        //получение данных от клиента
                        do
                        {
                            bytes = handler.Receive(data);
                            builder.Append(Encoding.Unicode.GetString(data, 0, bytes));
                        }
                        while (handler.Available > 0);

                        //разбор данных от клиента
                        string response = builder.ToString();
                        string[] parseBuilder = response.Split(new char[] { '|' });

                        Console.WriteLine(DateTime.Now.ToShortTimeString() + ": " + builder.ToString());
                        //запрос от пользователя на отключение
                        if (parseBuilder[0] == "disconnect")
                        {
                            isConnected = false;
                        }
                        //запрос от пользователя на получение данных
                        else if (parseBuilder[0] == "get")
                        {
                            try
                            {
                                string[] fileFormat = parseBuilder[1].Split(new char[] { '.' });
                                
                                if (fileFormat.Length == 2)
                                {
                                    //открывается какой-либо файл
                                    if (fileFormat[1] == "txt")
                                    {
                                        //файл формата txt
                                        string message = GetFile(parseBuilder[1]);
                                        SendData(message, handler, "файл отправлен");
                                    }
                                    else
                                    {
                                        //файл формата не txt
                                        string messageErr = "err|невозможно открыть файл";
                                        SendData(messageErr, handler, "невозможно открыть файл");
                                    }
                                }
                                else
                                {
                                    //открывается папка
                                    if (parseBuilder.Length > 1 && parseBuilder[1].Length > 0)
                                        directory = parseBuilder[1];
                                    string message = GetAllInDir(directory);
                                    SendData(message, handler, message);
                                }
                            }
                            catch (Exception ex)
                            {
                                string messageErr = "err|" + ex.Message;
                                SendData(messageErr, handler, messageErr);
                            }
                        }
                        else
                        {
                            //ответ на случай, если клиент отправил неподохдящий запрос
                            string message = "ваше сообщение доставлено";
                            data = Encoding.Unicode.GetBytes(message);
                            handler.Send(data);
                        }
                    }
                    // закрываем сокет

                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
