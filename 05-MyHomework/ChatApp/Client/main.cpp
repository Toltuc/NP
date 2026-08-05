// ChatApp Client - самостоятельное решение ДЗ по сетевому программированию.
// Требование: "У клиента должен быть конфигурационный файл, в котором записан
// адрес Сервера" -> читаем адрес и порт из Client.ini рядом с exe.
//
// Client.ini формат (два токена через пробел или на разных строках):
//   127.0.0.1 27015

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <iostream>

#include "../include/FormatLastError.h"

#pragma comment(lib, "Ws2_32.lib")

using std::cin;
using std::cout;
using std::endl;

constexpr int MTU = 1500;
volatile bool g_finished = false;

DWORD WINAPI ReceiveLoop(LPVOID param)
{
    SOCKET socketHandle = (SOCKET)(intptr_t)param;
    CHAR recvBuffer[MTU] = {};
    CHAR szError[256] = {};

    while (!g_finished)
    {
        ZeroMemory(recvBuffer, MTU);
        int result = recv(socketHandle, recvBuffer, MTU - 1, 0);

        if (result > 0)
        {
            cout << "\n" << recvBuffer << "\n> ";
            cout.flush();
        }
        else if (result == 0)
        {
            cout << "\n[Клиент] Соединение закрыто сервером." << endl;
            break;
        }
        else
        {
            if (WSAGetLastError() == WSAECONNRESET) break;
            cout << "recv() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
            break;
        }
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    cout << "=== ChatApp CLIENT ===" << endl;

    std::string serverIp;
    std::string serverPort;
    std::ifstream configFile("Client.ini");
    if (configFile.is_open())
    {
        configFile >> serverIp >> serverPort;
        configFile.close();
    }

    if (serverIp.empty() || serverPort.empty())
    {
        cout << "Ошибка: 'Client.ini' не найден или пуст. Формат: <ip> <port>" << endl;
        return 1;
    }
    cout << "Сервер из конфигурации: " << serverIp << ":" << serverPort << endl;

    CHAR szError[256] = {};
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    addrinfo hints = {};
    addrinfo* target = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(serverIp.c_str(), serverPort.c_str(), &hints, &target);
    if (iResult != 0)
    {
        cout << "getaddrinfo() failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    SOCKET connectSocket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
    if (connectSocket == INVALID_SOCKET)
    {
        cout << "socket() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        freeaddrinfo(target);
        WSACleanup();
        return 1;
    }

    iResult = connect(connectSocket, target->ai_addr, (int)target->ai_addrlen);
    freeaddrinfo(target);
    if (iResult == SOCKET_ERROR)
    {
        cout << "connect() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Подключено к серверу." << endl;
    HANDLE receiveThread = CreateThread(NULL, 0, ReceiveLoop, (LPVOID)(intptr_t)connectSocket, 0, NULL);

    std::string line;
    do
    {
        cout << "> ";
        std::getline(cin, line);
        if (line.empty()) continue;

        int result = send(connectSocket, line.c_str(), (int)line.size(), 0);
        if (result == SOCKET_ERROR)
        {
            cout << "send() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
            break;
        }
    } while (line != "exit");

    g_finished = true;
    shutdown(connectSocket, SD_BOTH);
    WaitForSingleObject(receiveThread, INFINITE);
    CloseHandle(receiveThread);

    closesocket(connectSocket);
    WSACleanup();
    return 0;
}
