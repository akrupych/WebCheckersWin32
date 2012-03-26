#include "stdafx.h"
#include "server.h"

// static fields declarations
bool Server::connected, Server::received;
SOCKET Server::ListenSocket, Server::ClientSocket;
addrinfo* Server::result;
char* Server::dataReceived;
char* Server::clientHost;

// Create a SOCKET object called ListenSocket for the server
// to listen for client connections.
int Server::CreateSocket(char* port)
{
    addrinfo *ptr = NULL, hints;
    result = NULL;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;			// IPv4 address family
    hints.ai_socktype = SOCK_STREAM;	// stream socket
    hints.ai_protocol = IPPROTO_TCP;	// TCP protocol
    // AI_PASSIVE flag indicates the caller intends to use the returned
    // socket address structure in a call to the bind function.
    hints.ai_flags = AI_PASSIVE;

    // When the AI_PASSIVE flag is set and nodename parameter to
    // the getaddrinfo function is a NULL pointer, the IP address
    // portion of the socket address structure is set to INADDR_ANY
    int iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0)
    {
        WSACleanup();
        return iResult;
    }

    ListenSocket = INVALID_SOCKET;
    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        iResult = WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return iResult;
    }

    return 0;
}

// For a server to accept client connections, it must be bound to a network address
// within the system. The following code demonstrates how to bind a socket
// that has already been created to an IP address and port.
int Server::BindSocket()
{
    if (ListenSocket == INVALID_SOCKET || result == NULL) return 1;

    // Setup the TCP listening socket
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        iResult = WSAGetLastError();
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return iResult;
    }

    // Free the memory allocated by the getaddrinfo function result address info.
    freeaddrinfo(result);

    return 0;
}

// After the socket is bound to an IP address and port on the system, the server
// must then listen on that IP address and port for incoming connection requests.
int Server::ListenForConnection()
{
    if (ListenSocket == INVALID_SOCKET) return 1;

    // SOMAXCONN allows a maximum reasonable number of pending connections in queue.
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        int iResult = WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
        return iResult;
    }

    return 0;
}

// On connection request application calls accept function
// and passes the work to another thread.
int Server::AcceptConnection()
{
    if (ListenSocket == INVALID_SOCKET) return 1;

    // temporary SOCKET object for accepting connections from clients.
    ClientSocket = INVALID_SOCKET;

    _beginthread(WaitForAccept, 0, (void*)ClientSocket);

    return 0;
}

void Server::WaitForAccept(void* arg)
{
    connected = false;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        int iResult = WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
    }

	while (true)
		if (Receive() == 0)
			connected = true;
}

// Receive data and send response
int Server::Receive()
{
    if (ClientSocket == INVALID_SOCKET) return 1;
	int iResult, iSendResult;
	char buffer[100];

    // Receive until the peer shuts down the connection
    while (true)
    {
		//while (!canReceive);	// don't receive until the end of our turn
        // returns an integer value of the number of bytes received, or an error
        iResult = recv(ClientSocket, buffer, sizeof(buffer), 0);
		//canReceive = false;		// socket is now used to transfer own turn to the enemy
        if (iResult > 0)
        {
			if (!connected)
			{
				clientHost = new char[iResult + 1];
				memcpy(clientHost, buffer, iResult);
				clientHost[iResult] = 0;
				connected = true;
			}
            // Echo the buffer back to the sender
            /*iSendResult = send(ClientSocket, hostname, strlen(hostname), 0);
            if (iSendResult == SOCKET_ERROR)
            {
                iSendResult = WSAGetLastError();
                closesocket(ClientSocket);
                WSACleanup();
                return iSendResult;
            }*/
			//if (!connected && buffer[0] == '1') connected = true;
			else if (!received)
			{
				delete [] dataReceived;
				dataReceived = new char[iResult + 1];
				memcpy(dataReceived, buffer, iResult);
				dataReceived[iResult] = 0;
				if (dataReceived) received = true;
			}
        }
        else if (iResult == 0) return 0;	// client connection is closed
        else								// error occured
        {
            iResult = WSAGetLastError();
            closesocket(ClientSocket);
            WSACleanup();
            return iResult;
        }
    }
}

// When the server is done sending data to the client, the shutdown function
// can be called specifying SD_SEND to shutdown the sending side of the socket.
void Server::Disconnect()
{
    if (ClientSocket == INVALID_SOCKET) return;
    // When the client application is done receiving data,
    // the closesocket function is called to close the socket.
    closesocket(ClientSocket);
    // When the client application is completed using the Windows Sockets DLL,
    // the WSACleanup function is called to release resources.
    WSACleanup();
}

// All processes that call Winsock functions must initialize the use
// of the WinSockets DLL before making other Winsock functions calls.
Server::Server()
{
    WSADATA wsaData;
    WSA_ready = !WSAStartup(MAKEWORD(2, 2), &wsaData);
	dataReceived = new char;
	//canReceive = true;
	clientHost = NULL;
}

// start server listening on port
int Server::Seed(char* port)
{
    int result;
    if (result = CreateSocket(port)) return result;
    if (result = BindSocket()) return result;
    if (result = ListenForConnection()) return result;
    if (result = AcceptConnection()) return result;
    //if (result = Receive(1)) return result;
    //Disconnect();
    return 0;
}
