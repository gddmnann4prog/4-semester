using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace Client
{
    public partial class Form1 : Form
    {
        // адрес и порт сервера, к которому будем подключаться
        static int port = 8005; // порт сервера
        static string address = "127.0.0.1"; // адрес сервера

        private IPEndPoint ipPoint;
        private Socket socket;

        private void CreateConnection()
        {
            try
            {
                ipPoint = new IPEndPoint(IPAddress.Parse(address), port);
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.Connect(ipPoint);
                textBox2.Text += DateTime.Now.ToShortTimeString() + ": cоединение с сервером установлено" + Environment.NewLine;
            }
            catch(Exception ex)
            {
                textBox2.Text += "Ошибка: " + ex.Message + Environment.NewLine;
            }
        }

        private void Disconnect()
        {
            string message = "disconnect";
            byte[] data = Encoding.Unicode.GetBytes(message);
            socket.Send(data);

            textBox2.Text += DateTime.Now.ToShortTimeString() + ": cоединение с сервером отключено" + Environment.NewLine;
            socket.Shutdown(SocketShutdown.Both);
            socket.Close();
        }

        private void ParseResponse(StringBuilder builder)
        {
            string response = builder.ToString();

            string[] items = response.Split(new char[] { '|' });

            switch (items[0])
            {
                case "dir":
                    {
                        textBox1.Text = items[1];
                        listBox1.Items.Clear();
                        for (int i = 2; i < items.Length; i++)
                        {
                            listBox1.Items.Add(items[i]);
                        }

                        break;
                    }
                case "txt":
                    {
                        textBox2.Text += Environment.NewLine + DateTime.Now.ToShortTimeString() + ": вывод файла " + items[1] + Environment.NewLine;
                        for (int i = 2; i < items.Length; i++)
                            textBox2.Text += items[i];

                        textBox2.Text += Environment.NewLine;

                        break;
                    }
                case "err":
                    {
                        textBox2.Text += DateTime.Now.ToShortTimeString() + ": " + items[1] + Environment.NewLine;

                        break;
                    }
            }
        }

        private void GoToParentDir(ref string path)
        {
            //возврат в папку уровнем выше
            string[] items = path.Split(new char[] { '\\' });

            path = "";
            for (int i = 0; i < items.Length - 1; i++)
            {
                if (i > 0)
                    path += "\\";
                path += items[i];
            }
        }

        private string CreateHeader()
        {
            string message = "get|" + textBox1.Text;
            if (listBox1.Items.Count > 0 && listBox1.SelectedItem != null)
                if (listBox1.SelectedItem.ToString() != "..")
                    message += "\\" + listBox1.SelectedItem.ToString();
                else
                    GoToParentDir(ref message);

            return message;
        }

        private StringBuilder GetResponse()
        {
            byte[] data = new byte[256]; // буфер для ответа
            StringBuilder builder = new StringBuilder();
            int bytes = 0; // количество полученных байт

            //записываем данные с сервера
            do
            {
                bytes = socket.Receive(data, data.Length, 0);
                builder.Append(Encoding.Unicode.GetString(data, 0, bytes));
            }
            while (socket.Available > 0);

            return builder;
        }

        private void SendQuery()
        {
            try
            {
                //заголовок запроса на сервер
                string message = CreateHeader();

                //отправляем запрос
                byte[] data = Encoding.Unicode.GetBytes(message);
                socket.Send(data);

                //получаем ответ
                StringBuilder builder = GetResponse();

                textBox2.Text += DateTime.Now.ToShortTimeString() + ": получен ответ" + Environment.NewLine;
                //разбираем ответ сервера
                ParseResponse(builder);
            }
            catch (Exception ex)
            {
                textBox2.Text += "Ошибка: " + ex.Message + Environment.NewLine;
            }

        }


        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                CreateConnection();
            }
            catch (Exception ex)
            {
                textBox2.Text += "Ошибка: " + ex.Message + Environment.NewLine;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                Disconnect();
            }
            catch (Exception ex)
            {
                textBox2.Text += "Ошибка: " + ex.Message + Environment.NewLine;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SendQuery();
        }

        private void listBox1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            SendQuery();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            textBox2.Text = "";
        }
    }
}
