#include "stdafx.h"
#include "Game.h"

// initialize field and set own turn flag
bool Game::Start()
{
	if (!server.Connected()) return false;
	if (!client.Connected())
		if (client.SetConnection(server.GetClientHost()) != 0)
			return false;
	myTurn = true;
	return field.Initialize();
}

// tries moving checker from one cell to another (maybe with beats),
// and if succeeds, sends info to another player and starts his turn
bool Game::MakeTurn(Cell from, Cell to)
{
	if (!myTurn) return false;
	byte* result = field.TryComplexMove(from, to);
	if (result) return GiveTurnToEnemy(result);
	return false;
}

// sends own turn info to an enemy and gives him a turn
bool Game::GiveTurnToEnemy(byte* myTurnInfo)
{
	if (client.SendData(myTurnInfo, strlen(myTurnInfo)) != 0) return false;
	myTurn = false;
	server.WaitForData();
	return true;
}

// try connecting to another player's game
bool Game::ConnectTo(char* serverHost)
{
	if (client.SearchFor(serverHost) != 0) return false;
	server.SetConnected();
	myTurn = false;
	return field.Initialize();
}

// checks for the enemie's turn end and updates field
bool Game::GetTurnFromEnemy()
{
	if (!server.Received()) return false;
	field.Update(server.GetData());
	myTurn = true;
	return true;
}