
#define _WINSOCK_DEPRECATED_NO_WARNINGS



#include < winsock2.h > 
#include < ws2tcpip.h >
#include < iostream >
#include <string>
#include <winsock.h>
#include <windows.h>
#include <fstream>
#include <list>
#include <iterator>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512									// размер буфера принимаемых данных
#define DEFAULT_PORT "1212"									// порт 
#define DEFAULT_FILENAME "E:\\test\\data.txt"	// путь до файла с данными

using namespace std;

char* getServerAddress()
{
	const int WSVer = 0x101;
	WSAData wsaData;
	hostent *h;
	char Buf[128];
	if (WSAStartup(WSVer, &wsaData) == 0)
	{
		if (gethostname(&Buf[0], 128) == 0)
		{
			h = gethostbyname(&Buf[0]);
			if (h != NULL)
			{
				//std::string addr 
				char *serverAddres = inet_ntoa(*(reinterpret_cast<in_addr *>(*(h->h_addr_list))));
				WSACleanup;
				return serverAddres;
				//MessageBox(0, serverAddres, 0, 0);

			}

			else
			{
				WSACleanup;
				return NULL;
				//MessageBox(0, "Вы не в сети. И IP адреса у вас нет.", 0, 0);
			}
		}
		
	}
}

std::string getMessage(SOCKET ClientSocket)
{
	char recvbuf[DEFAULT_BUFLEN + 1];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	// Принимаем данные от клиента
	int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult == 0)
	{
		std::cout << "Соединение закрыто..." << std::endl;
		system("pause");
		exit(0);
	}
	else if (iResult < 0)
	{
		std::cout << "Ошибка recv " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		system("pause");
		exit(0);
	}

	recvbuf[iResult] = '\0';	// чтобы мусор не принимать
	std::string returnBuf(recvbuf);
	return returnBuf;
	//std::cout << "Сообщение: " << recvbuf << " | Размер: " << strlen(recvbuf) << std::endl;
}

int getAmmountOfLinesInFile(std::string fileName)
{
	int linesCounter = 0;
	std::string line;
	std::ifstream inputFileStream(fileName);

	if (!inputFileStream.is_open())
		return 0;

	while (!inputFileStream.eof())
	{
		getline(inputFileStream, line);
		linesCounter++;
	}
	inputFileStream.close();
	return linesCounter;
}

bool rewriteLineInFile(std::string fileName, std::string newLine, int position)
{
	std::list <std::string> lineList;
	std::ifstream inputFileStream(fileName);
	int lineCounter = 0;

	if (!inputFileStream.is_open())
		return 0;

	std::string str;

	while (!inputFileStream.eof())
	{
		getline(inputFileStream, str);
		lineList.push_back(str);
		lineCounter++;
	}
	inputFileStream.close();


	std::list <std::string> ::iterator lineListIterator = lineList.begin();
	std::ofstream outputFileStream(fileName);
	if (!outputFileStream.is_open())
		return 0;

	for (int i = 0; i < lineCounter; i++)
	{
		if (i == position - 1)
			outputFileStream << newLine;
		else
			outputFileStream << *lineListIterator;
		if (i != lineCounter - 1)
			outputFileStream << std::endl;
		lineListIterator++;
	}

	/*
	while (!inputFileStream.eof())
	{
		if (lineCounter == position)
			outputFileStream << newLine << std::endl;
		else
			outputFileStream << *lineListIterator << std::endl;
		std::cout << *lineListIterator << std::endl;
		lineCounter--;
	}
	*/
	/*
	for (int i = 0; i <= position; i++)
	{
		if (i == position)
			*lineListIterator = newLine;
		else

		lineListIterator--;
	}

	*lineListIterator = newLine;
	
	*/

	inputFileStream.close();
	//std::cout << *lineListIterator << std::endl;
	
	return true;
}


void startSocketServer()
{
	// получаем данные базы
	int lineCounter = getAmmountOfLinesInFile(DEFAULT_FILENAME);
	if (!lineCounter)
	{
		std::cout << "Произошла ошибка при получении доступа к базе данных." << std::endl;
		return;
	}

	// инициализируем Windows Socket API
	int iResult;

	WSAData d;
	iResult = WSAStartup(MAKEWORD(2, 2), &d);
	if (iResult != 0)
	{
		std::cout << "Ошибка инициализации WSAStartup: " << iResult;
		return;
	}

	// готовим данные для создания сокета


	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof (hints));
	// заполняем структуру параметрами (протокол (IPv4), режим взаимодействия (TCP), протокол (TCP), IP-адрес)
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// по заданным параметрам заполняется структура result типа addrinfo
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "Ошибка getaddrinfo: " << iResult;
		WSACleanup();
		return;
	}

	// создаем сокет
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "Ошибка создания сокета: " << WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// связываем сокет с адресом (ip:port)
	iResult = bind(listenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Ошибка установки связи с адреоом: " << WSAGetLastError();
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result); // больше переменная не нужна, пожтому освобождаем

	// прослушиваем подключения
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Ошибка при прослушивании подключения: " << WSAGetLastError();
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	std::cout << "Адрес сервера: " << getServerAddress() << std::endl;
	std::cout << "Ожидание подключения." << std::endl;

	// обрабатываем запрос на подключение
	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(listenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "Ошибка при обработке запроса: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}




	// обмениваемся данными
	char recvbuf[DEFAULT_BUFLEN + 1];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	
		// Принимаем данные от клиента
		/*
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult == 0)
		{
			std::cout << "Соединение закрыто..." << std::endl;
			return;
		}
		 else if (iResult < 0)
		{
			std::cout << "Ошибка recv " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

		recvbuf[iResult] = '\0';	// чтобы мусор не принимать
		std::cout << "Сообщение: " << recvbuf << " | Размер: " << strlen(recvbuf) << std::endl;
		*/

		


		 // Отправляем данные
			// отправляем количество строк в базе
		std::string dataStr = std::to_string(lineCounter);
		const char *data = dataStr.c_str();
		
		 iSendResult = send(ClientSocket, data, static_cast< int >(strlen(data)), 0);
		 if (iSendResult == SOCKET_ERROR)
		 {
			 std::cout << "Ошибка отправки данных: " << WSAGetLastError() << std::endl;
			 closesocket(ClientSocket);
			 WSACleanup();
			 return;
		 }

		 // принимаем номер строки для замены
		 int lineNumber = std::stoi(getMessage(ClientSocket));

		 // принимае значение новой строки
		 std::string newString = getMessage(ClientSocket);

		 std::cout << "Получены значения: " << std::endl;
		 std::cout << "Номер заменяемой строки: " << lineNumber << std::endl;
		 std::cout << "Новое значение строки: " << newString << std::endl;

		 if (rewriteLineInFile(DEFAULT_FILENAME, newString, lineNumber))
			 std::cout << "Строка перезаписана." << std::endl;
		 else
			 std::cout << "Произошла ошибка при перезаписи строки." << std::endl;
}


void main()
{
	setlocale(LC_ALL, "Russian");
	std::cout << "Программа передачи данных: (Сервер)" << std::endl;
	startSocketServer();
	system("pause");
	return;
}