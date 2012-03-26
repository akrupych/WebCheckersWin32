#include "stdafx.h"
#include "Field.h"
#include "Client.h"
#include "Server.h"

// checkers game class
class Game
{

protected:

	Field field;		// game field

	Client client;		// client socket
	Server server;		// server socket

	bool serverStarted; // is server listening
	bool myTurn;		// can I move checkers now

public:

	// get cells byte representation:
	// 0 - empty, 1 - own checker, -1 - enemy checker
	byte* GetField() { return field.ToBytes(); }

	// starts server seeding in a new thread
	bool SeedGame() { return serverStarted = !server.Seed(); }

	// try connecting to another player's game
	bool ConnectTo(char* server);

	// init field and set own move flag
	bool Start();

	// checks if I can move checkers now
	bool IsMyTurn() const { return myTurn; }

	// tries moving checker from one cell to another (maybe with beats),
	// and if succeeds sends info to another player and starts his turn
	bool MakeTurn(Cell from, Cell to);

	// sends own turn info to an enemy and gives him a turn
	bool GiveTurnToEnemy(byte* myTurnInfo);
	
	// checks for the enemie's turn end and updates field
	bool GetTurnFromEnemy();

	char* GetEnemyHost() { return server.GetClientHost(); }
};