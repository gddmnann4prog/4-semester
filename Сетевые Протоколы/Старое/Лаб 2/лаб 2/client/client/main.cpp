#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include < winsock2.h > 
#include < ws2tcpip.h >
#include < iostream >
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define maxStringLength 40

#define port 1212					//	����
#define bufsize 512				//	������ ������ ����������� ������

char* getMessage(SOCKET clientSocket)
{
	// ��������� ������
	char buf[bufsize + 1];
	int r;
	do
	{
		r = recv(clientSocket, buf, bufsize, 0);
		if (r > 0)
		{
			buf[r] = '\0';		// ����� ����� �� ���������
			return buf;
			//std::cout << "���������: " << buf << " | ������: " << strlen(buf) << std::endl;
		}
		else if (r == 0)
		{
			std::cout << "���������� ���������" << std::endl;
			system("pause");
			exit(0);
		}
		else
		{
			std::cout << "������ � recv(): " << WSAGetLastError() << std::endl;
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
		std::cout << "������ � send(): " << WSAGetLastError() << std::endl;
		system("pause");
		exit(0);
	}
}

void startSocketClient()
{
	// �������������� Windows Socket API
	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cout << "������ WSAStartup: " << result << std::endl;
		return;
	}

	// ������� �����
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "������ socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// ������������ � �������
	//#define ip "127.0.0.1"
	std::cout << "������� ����� �������: ";
	std::string serverAddress;
	getline(std::cin, serverAddress);
	if (::inet_addr(serverAddress.c_str()) == INADDR_NONE)
	{
		std::cout << "�� ����� �� ������������ IP �����." << std::endl;
		return;
	}



	// ��������� ��������� ����������� ��� ��������� (�������� (IPv4), ip, ����)
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	clientService.sin_port = htons(port);

	// ������������, ��������� ���������� ������, ��������� � ����������� � �� ������
	result = connect(
		clientSocket,
		reinterpret_cast< SOCKADDR* >(&clientService),
		sizeof(clientService)
		);
	if (result != 0)
	{
		std::cout << "������ � connect(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	

	// �������� ������ �� ������
	/*
	char data[] = "12345";
	result = send(clientSocket, data, static_cast< int >(strlen(data)), 0);
	if (result < 0)
	{
		std::cout << "������ � send(): " << WSAGetLastError() << std::endl;
		return;
	}
	*/


	// -------------------- ������� ������� � �������� ------------------------------------------------------
	char* lineCounterChar = getMessage(clientSocket);
	int lineCounter = std::stoi(lineCounterChar);
	std::cout << "���������� ����� � ���� ������: " << lineCounter << std::endl;
	std::cout << "������� ����� ������ ��� ���������� (� �������� ������� [1;" << lineCounter << "]): ";
	std::string lineNumberStr;
	getline(std::cin, lineNumberStr);
	const char *lineNumberChar = lineNumberStr.c_str();
	if (!atoi(lineNumberChar))
	{
		std::cout << "������. ���������� �������� �� �������� ������." << std::endl;
		return;
	}
	else
	{
		double lineNumber = atof(lineNumberChar);
		if (lineNumber < 1 || lineNumber > lineCounter)
		{
			std::cout << "������. ������ � ����� ������� � ���� ������ �� ����������." << std::endl;
			return;
		}

		for (int i = 0; i < strlen(lineNumberChar); i++)
		{
			if (lineNumberChar[i] == '.')
			{
				std::cout << "������. ����� ������ ���� ����� ������." << std::endl;
				return;
			}
		}
	}

	// �������� ����� ������
	sendMessage(clientSocket, lineNumberStr);

	/*
	result = send(clientSocket, lineNumberChar, static_cast< int >(strlen(lineNumberChar)), 0);
	if (result < 0)
	{
		std::cout << "������ � send(): " << WSAGetLastError() << std::endl;
		return;
	}
	*/

	// �������� �������� ����� ������
	std::cout << "������� ����� ������ (�� ������� " << maxStringLength << " ��������): ";
	std::string newString;
	getline(std::cin, newString);
	if (newString.length() > maxStringLength)
	{
		std::cout << "������. ����� ����� ������ ��������� ������������ (" << maxStringLength << ")." << std::endl;
		return;
	}
	sendMessage(clientSocket, newString);

	// ��������� ������
	/*
	char buf[bufsize + 1];
	int r;
	do
	{
		r = recv(clientSocket, buf, bufsize, 0);
		if (r > 0)
		{
			buf[r] = '\0';		// ����� ����� �� ���������
			std::cout << "���������: " << buf << " | ������: " << strlen(buf) << std::endl;
		}
		else if (r == 0)
			std::cout << "���������� ���������" << std::endl;
		else
			std::cout << "������ � recv(): " << WSAGetLastError() << std::endl;
	} while (r > 0);
	*/
	// ��������� ����������
	closesocket(clientSocket);
	WSACleanup(); // ���� ������ � �������� ������ �� �������������� �������� WSACleanup()
}



int main()
{

	setlocale(LC_ALL, "Russian");
	std::cout << "��������� �������� ������: (������)" << std::endl;
	startSocketClient();
	system("pause");
	return 0;
}