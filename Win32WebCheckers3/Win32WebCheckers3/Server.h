#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

class Server
{
    bool WSA_ready;

    static SOCKET ListenSocket, ClientSocket;	// sockets
	static bool connected;						// another player connected
	static bool received;						// another player turn received
	//static bool canReceive;						// is my turn sent to the enemy

    static addrinfo *result;					// used by sockets
    static char* dataReceived;					// data received from client

	static char* clientHost;

    // Create a SOCKET object called ListenSocket for the server
    // to listen for client connections.
    int CreateSocket(char* port);

    // For a server to accept client connections, it must be bound to a network address
    // within the system. The following code demonstrates how to bind a socket
    // that has already been created to an IP address and port.
    int BindSocket();

    // After the socket is bound to an IP address and port on the system, the server
    // must then listen on that IP address and port for incoming connection requests.
    int ListenForConnection();

    // On connection request application calls accept function
    // and passes the work to another thread.
    int AcceptConnection();

    static void WaitForAccept(void *arg);

    // Receive data and send response
    static int Receive();

    // When the server is done sending data to the client, the shutdown function
    // can be called specifying SD_SEND to shutdown the sending side of the socket.
    void Disconnect();

public:

    // All processes that call Winsock functions must initialize the use
    // of the WinSockets DLL before making other Winsock functions calls.
    Server();

	// start server listening on port in a new thread
    int Seed(char *port = "1234");

	// return received data
    inline char* GetData() const { return dataReceived; }

	// check whether another player connected
	bool Connected() const { return connected; }
	
	// check whether another player had finished his turn
	bool Received() const { return received; }

	// reset receiving flag
	void WaitForData() { received = false; /*canReceive = true;*/ }

	char* GetClientHost() { return clientHost; }

	void SetConnected() { connected = true; }
};
