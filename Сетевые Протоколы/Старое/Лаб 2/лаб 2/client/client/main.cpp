#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include < winsock2.h > 
#include < ws2tcpip.h >
#include < iostream >
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define maxStringLength 40

#define port 1212					//	порт
#define bufsize 512				//	размер буфера принимаемых данных

char* getMessage(SOCKET clientSocket)
{
	// принимаем данные
	char buf[bufsize + 1];
	int r;
	do
	{
		r = recv(clientSocket, buf, bufsize, 0);
		if (r > 0)
		{
			buf[r] = '\0';		// чтобы мусор не принимать
			return buf;
			//std::cout << "Сообщение: " << buf << " | Размер: " << strlen(buf) << std::endl;
		}
		else if (r == 0)
		{
			std::cout << "Соединение разорвано" << std::endl;
			system("pause");
			exit(0);
		}
		else
		{
			std::cout << "Ошибка в recv(): " << WSAGetLastError() << std::endl;
			system("pause");
			exit(0);
		}
	} while (r > 0);
}
void sendMessage(SOCKET clientSocket, std::string message)
{
	const char *messageChar = message.c_str();
	int result = send(clientSocket, messageChar, static_cast< int >(strlen(messageChar)), 0);
	if (result < 0)
	{
		std::cout << "Ошибка в send(): " << WSAGetLastError() << std::endl;
		system("pause");
		exit(0);
	}
}

void startSocketClient()
{
	// инициализируем Windows Socket API
	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cout << "Ошибка WSAStartup: " << result << std::endl;
		return;
	}

	// создаем сокет
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Ошибка socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// подключаемся к серверу
	//#define ip "127.0.0.1"
	std::cout << "Введите адрес сервера: ";
	std::string serverAddress;
	getline(std::cin, serverAddress);
	if (::inet_addr(serverAddress.c_str()) == INADDR_NONE)
	{
		std::cout << "Вы ввели не существующий IP адрес." << std::endl;
		return;
	}



	// заполняем структуру параметрами для сединения (проторол (IPv4), ip, порт)
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	clientService.sin_port = htons(port);

	// подключаемся, передавая дескриптор сокета, структуру с параметрами и ее размер
	result = connect(
		clientSocket,
		reinterpret_cast< SOCKADDR* >(&clientService),
		sizeof(clientService)
		);
	if (result != 0)
	{
		std::cout << "Ошибка в connect(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	

	// передаем данные на сервер
	/*
	char data[] = "12345";
	result = send(clientSocket, data, static_cast< int >(strlen(data)), 0);
	if (result < 0)
	{
		std::cout << "Ошибка в send(): " << WSAGetLastError() << std::endl;
		return;
	}
	*/


	// -------------------- Общение клиента с сервером ------------------------------------------------------
	char* lineCounterChar = getMessage(clientSocket);
	int lineCounter = std::stoi(lineCounterChar);
	std::cout << "Количество строк в базе данных: " << lineCounter << std::endl;
	std::cout << "Введите номер строки для перезаписи (в пределах отрезка [1;" << lineCounter << "]): ";
	std::string lineNumberStr;
	getline(std::cin, lineNumberStr);
	const char *lineNumberChar = lineNumberStr.c_str();
	if (!atoi(lineNumberChar))
	{
		std::cout << "Ошибка. Полученное значение не является числом." << std::endl;
		return;
	}
	else
	{
		double lineNumber = atof(lineNumberChar);
		if (lineNumber < 1 || lineNumber > lineCounter)
		{
			std::cout << "Ошибка. Строки с таким номером в базе данных не существует." << std::endl;
			return;
		}

		for (int i = 0; i < strlen(lineNumberChar); i++)
		{
			if (lineNumberChar[i] == '.')
			{
				std::cout << "Ошибка. Номер должен быть целым числом." << std::endl;
				return;
			}
		}
	}

	// передаем номер строки
	sendMessage(clientSocket, lineNumberStr);

	/*
	result = send(clientSocket, lineNumberChar, static_cast< int >(strlen(lineNumberChar)), 0);
	if (result < 0)
	{
		std::cout << "Ошибка в send(): " << WSAGetLastError() << std::endl;
		return;
	}
	*/

	// передаем значение новой строке
	std::cout << "Введите новую строку (не длиннее " << maxStringLength << " символов): ";
	std::string newString;
	getline(std::cin, newString);
	if (newString.length() > maxStringLength)
	{
		std::cout << "Ошибка. Длина новой строки превышает максимальную (" << maxStringLength << ")." << std::endl;
		return;
	}
	sendMessage(clientSocket, newString);

	// принимаем данные
	/*
	char buf[bufsize + 1];
	int r;
	do
	{
		r = recv(clientSocket, buf, bufsize, 0);
		if (r > 0)
		{
			buf[r] = '\0';		// чтобы мусор не принимать
			std::cout << "Сообщение: " << buf << " | Размер: " << strlen(buf) << std::endl;
		}
		else if (r == 0)
			std::cout << "Соединение разорвано" << std::endl;
		else
			std::cout << "Ошибка в recv(): " << WSAGetLastError() << std::endl;
	} while (r > 0);
	*/
	// закрываем соединение
	closesocket(clientSocket);
	WSACleanup(); // Если работа с сокетами больше не предполагается вызываем WSACleanup()
}



int main()
{

	setlocale(LC_ALL, "Russian");
	std::cout << "Программа передачи данных: (Клиент)" << std::endl;
	startSocketClient();
	system("pause");
	return 0;
}