#include "stdafx.h"
#include "client.h"

Client::Client()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    WSA_ready = !iResult;
	ConnectSocket = INVALID_SOCKET;
}

int Client::Find(char* server, char* port)
{
    if (!WSA_ready) return 1;

    addrinfo hints;
    result = ptr = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(server, port, &hints, &result);
    if ( iResult != 0 )
    {
        WSACleanup();
        return iResult;
    }

    return 0;
}

int Client::Connect()
{
    if (!WSA_ready) return 1;
    ConnectSocket = INVALID_SOCKET;

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            WSACleanup();
            return INVALID_SOCKET;
        }

        // Connect to server.
        int iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (result) freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return INVALID_SOCKET;
    }
    return 0;
}

int Client::Send(char* data, unsigned size)
{
    if (!WSA_ready) return 1;

    // Send an initial buffer
    int iResult = send(ConnectSocket, data, size, 0 );
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        WSACleanup();
        return iResult;
    }

    // shutdown the connection since no more data will be sent
    /*iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        WSACleanup();
        return iResult;
    }*/

    // Receive confirm
    /*char* received = new char[1];
    iResult = recv(ConnectSocket, received, 1, 0);
    if (iResult <= 0 || received[0] != '1') return 1;*/

    return 0;
}

void Client::Close()
{
    if (!WSA_ready) return;

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

int Client::SearchFor(char* server, char* port)
{
    if (strlen(server) == 0) return 1;
	
	// retrieve own hostname to send to another player
	char hostname[100];
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
		return WSAGetLastError();

	// find server, connect and send own hostname for back connection
    int result;
    if ((result = Find(server, port)) ||
            (result = Connect()) ||
			(result = Send(hostname, strlen(hostname))))
			return result;
	return 0;
}

int Client::SendData(char* data, unsigned size)
{
	return Send(data, size);
}

int Client::SetConnection(char* serverHost)
{
	int result = Find(serverHost);
	if (result == 0) return Connect();
}