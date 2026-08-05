// ChatApp Server - самостоятельное решение ДЗ по сетевому программированию.
// Требования, взятые из ReadMe.txt курса:
//   1. При отключении клиента освобождается место для нового подключения;
//   2. На Сервере отображается количество подключенных клиентов;
//   3. Сообщение, отправленное на Сервер, пересылается всем подключенным клиентам;
//   4. Вместо кода ошибки программа выводит сообщение об ошибке (FormatLastError);
//   5. Запускается на виртуальной машине / любом ПК в локальной сети.
//
// Сборка (Visual Studio, x64/x86): подключить Ws2_32.lib, include/FormatLastError.cpp.
// Порт по умолчанию: 27015.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <mutex>
#include <vector>

#include "../include/FormatLastError.h"

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::endl;

constexpr int SERVER_PORT = 27015;
constexpr int MAX_CLIENTS = 10;
constexpr int MTU = 1500;

struct ClientSlot
{
    SOCKET socket = INVALID_SOCKET;
    HANDLE thread = NULL;
    std::string address;
    bool used = false;
};

ClientSlot g_clients[MAX_CLIENTS];
std::mutex g_clientsMutex;
int g_clientCount = 0;

void PrintActiveClientCount()
{
    std::lock_guard<std::mutex> lock(g_clientsMutex);
    cout << "[Сервер] Подключено клиентов: " << g_clientCount << " / " << MAX_CLIENTS << endl;
}

// Рассылает сообщение всем клиентам, кроме отправителя (индекс excludeIndex).
void Broadcast(const char* message, int excludeIndex)
{
    std::lock_guard<std::mutex> lock(g_clientsMutex);
    CHAR szError[256] = {};

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!g_clients[i].used || i == excludeIndex) continue;

        int result = send(g_clients[i].socket, message, (int)strlen(message), 0);
        if (result == SOCKET_ERROR)
        {
            cout << "send() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        }
    }
}

int FindFreeSlot()
{
    std::lock_guard<std::mutex> lock(g_clientsMutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (!g_clients[i].used) return i;
    return -1;
}

DWORD WINAPI ClientHandler(LPVOID param)
{
    int slotIndex = (int)(intptr_t)param;
    SOCKET clientSocket = g_clients[slotIndex].socket;

    CHAR recvBuffer[MTU] = {};
    CHAR sendBuffer[MTU] = {};
    CHAR szError[256] = {};

    cout << "[Сервер] Клиент подключился: " << g_clients[slotIndex].address << endl;
    PrintActiveClientCount();

    int result = 0;
    do
    {
        ZeroMemory(recvBuffer, MTU);
        result = recv(clientSocket, recvBuffer, MTU - 1, 0);

        if (result > 0)
        {
            cout << g_clients[slotIndex].address << ": " << recvBuffer << endl;
            _snprintf_s(sendBuffer, MTU, _TRUNCATE, "%s: %s", g_clients[slotIndex].address.c_str(), recvBuffer);
            Broadcast(sendBuffer, slotIndex);
        }
        else if (result == 0)
        {
            cout << "[Сервер] Клиент отключился: " << g_clients[slotIndex].address << endl;
        }
        else
        {
            cout << "recv() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
            break;
        }
    } while (result > 0);

    closesocket(clientSocket);

    // Освобождаем слот - требование "при отключении клиента должно освобождаться место".
    {
        std::lock_guard<std::mutex> lock(g_clientsMutex);
        g_clients[slotIndex].used = false;
        g_clients[slotIndex].socket = INVALID_SOCKET;
        CloseHandle(g_clients[slotIndex].thread);
        g_clients[slotIndex].thread = NULL;
        g_clientCount--;
    }
    PrintActiveClientCount();

    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    cout << "=== ChatApp SERVER ===" << endl;

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
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, std::to_string(SERVER_PORT).c_str(), &hints, &target);
    if (iResult != 0)
    {
        cout << "getaddrinfo() failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    SOCKET listenSocket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "socket() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        freeaddrinfo(target);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSocket, target->ai_addr, (int)target->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        cout << "bind() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        freeaddrinfo(target);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(target);

    if (listen(listenSocket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        cout << "listen() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Сервер слушает порт " << SERVER_PORT << "..." << endl;

    const char* declineMessage = "SERVER FULL, TRY LATER\n";

    while (true)
    {
        sockaddr_in clientAddr = {};
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            cout << "accept() failed: " << FormatLastError(WSAGetLastError(), szError) << endl;
            continue;
        }

        int slotIndex = FindFreeSlot();
        if (slotIndex == -1)
        {
            send(clientSocket, declineMessage, (int)strlen(declineMessage), 0);
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            cout << "[Сервер] Отклонён клиент: достигнут лимит MAX_CLIENTS" << endl;
            continue;
        }

        CHAR addrText[32] = {};
        sprintf_s(addrText, "%s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        {
            std::lock_guard<std::mutex> lock(g_clientsMutex);
            g_clients[slotIndex].socket = clientSocket;
            g_clients[slotIndex].address = addrText;
            g_clients[slotIndex].used = true;
            g_clientCount++;
        }

        g_clients[slotIndex].thread = CreateThread(NULL, 0, ClientHandler, (LPVOID)(intptr_t)slotIndex, 0, NULL);
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
