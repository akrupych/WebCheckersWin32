#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class Client
{
    bool WSA_ready;
    addrinfo *result, *ptr;
    SOCKET ConnectSocket;

    int Find(char* server, char* port = "1234");

    int Connect();

    int Send(char* data, unsigned size);

    void Close();

public:

	Client();

    int SearchFor(char* server, char* port = "1234");

	int SendData(char* data, unsigned size);

	int SetConnection(char* serverHost);

	bool Connected() { return ConnectSocket != INVALID_SOCKET; }
};
