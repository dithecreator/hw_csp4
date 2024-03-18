
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;

DWORD WINAPI Sender(void* param)
{


    char name[DEFAULT_BUFLEN]; //���������� ��� ��������� �����
    // cout << "Please insert your query for server: ";
    cout << "enter your name: ";
    cin.getline(name, DEFAULT_BUFLEN);





    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // ������ ����� � ������������
    cout << "enter the color text: " << endl;
    cout << "1. red" << endl;
    cout << "2. green" << endl;
    cout << "3. blue" << endl;

    int choice;
    cin >> choice;


    while (true) {

        char query[DEFAULT_BUFLEN];
        cin.getline(query, DEFAULT_BUFLEN);



        // ���������� ��� � ������
        char fullNameAndQuery[2 * DEFAULT_BUFLEN]; // �������������� ������

 


        //strcpy_s(fullNameAndQuery, DEFAULT_BUFLEN, name); // �������� ��� � �������������� ������
        //strcat_s(fullNameAndQuery, DEFAULT_BUFLEN, ": "); // ��������� ����������� ����� ������ � ��������
        //strcat_s(fullNameAndQuery, DEFAULT_BUFLEN, query); // ��������� ������ � �������������� ������

        sprintf_s(fullNameAndQuery, "%d|%s|%s", choice, name, query);
        send(client_socket, fullNameAndQuery, strlen(fullNameAndQuery), 0);



        // �������������� ������� ����� ������ ��������
        // string query;
        // getline(cin, query);
        // send(client_socket, query.c_str(), query.size(), 0);
    }
}

DWORD WINAPI Receiver(void* param)
{
    while (true) {
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        response[result] = '\0';

        // cout << "...\nYou have new response from server: " << response << "\n";
        cout << response << "\n";
        // cout << "Please insert your query for server: ";
    }
}

BOOL ExitHandler(DWORD whatHappening)
{
    switch (whatHappening)
    {
    case CTRL_C_EVENT: // closing console by ctrl + c
    case CTRL_BREAK_EVENT: // ctrl + break
    case CTRL_CLOSE_EVENT: // closing the console window by X button
      return(TRUE);
        break;
    default:
        return FALSE;
    }
}

int main()
{
    // ���������� �������� ���� �������
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitHandler, true);

    system("title Client");

    // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // ��������� ����� ������� � ����
    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    // �������� ������������ � ������, ���� �� �������
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // ������� ����� �� ������� ������� ��� ����������� � �������
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (client_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        // connect to server
        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    CreateThread(0, 0, Sender, 0, 0, 0);
    CreateThread(0, 0, Receiver, 0, 0, 0);

    Sleep(INFINITE);
}