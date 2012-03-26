#include "stdafx.h"
#include "field.h"

// get cells byte representation:
// 0 - empty, 1 - own checker, -1 - enemy checker
byte* Field::ToBytes()
{
	byte* field = new byte[64];

	// empty field
	if (field)
		for (int i = 0; i < 64; i++)
			field[i] = 0;

	// set enemy checkers
	for (byte i = 0; i < myCheckers.Count(); i++)
		field[myCheckers[i]->row * 8 + myCheckers[i]->column] = myCheckers[i]->isQueen() ? 2 : 1;
	
	// set own checkers
	for (byte i = 0; i < enemyCheckers.Count(); i++)
		field[enemyCheckers[i]->row * 8 + enemyCheckers[i]->column] = enemyCheckers[i]->isQueen() ? -2 : -1;

	return field;
}

// set all checkers at initial positions
bool Field::Initialize()
{
	// clear all
	myCheckers.Clear();
	enemyCheckers.Clear();

	// set enemy checkers
	for (byte row = 0; row < 3; row++)
	{
		for (byte column = !(row % 2); column < 8; column += 2)
		{
			if (!enemyCheckers.Add(Cell(row, column)))
			{
				enemyCheckers.Clear();
				return false;
			}
		}
	}
	
	// set own checkers
	for (byte row = 5; row < 8; row++)
	{
		for (byte column = !(row % 2); column < 8; column += 2)
		{
			if (!myCheckers.Add(Cell(row, column)))
			{
				myCheckers.Clear();
				return false;
			}
		}
	}
}

// tries moving checker from one cell to another (maybe with beats)
// returns: NULL if failed, otherwise byte array to be sent to another player
byte* Field::TryComplexMove(Cell from, Cell to, byte* prevBuffer)
{
	// correctness checking
	if (!from.IsGood() || !to.IsGood()) return false;
	if (!from.IsBlack() || !to.IsBlack()) return false;
	if (!IsMyChecker(from) || !IsEmptyCell(to)) return false;

	// once preserve space enought to store info about all possible beats
	if (!prevBuffer)
	{
		if (!(prevBuffer = new byte[packetSize])) return NULL;
		for (int i = 0; i < packetSize; prevBuffer[i++] = 0);
	}
	byte* subResult;

	// if no beats before checker can simply move
	if (prevBuffer[0] == 0 && (subResult = TryMove(from, to)))
		return subResult;

	// try to beat enemy and land on "to" cell
	if (subResult = TryBeat(from, to, prevBuffer)) return subResult;

	// recursively beat enemy checkers
	List jumps = GetAvailableBeatJumps(from);
	int before;	// for backup
	for (int i = 0; i < jumps.Count(); i++)
	{
		before = strlen(prevBuffer);
		if (TryBeat(from, *jumps[i], prevBuffer))
		{
			// if we can beat, check for the next beat
			if (TryComplexMove(*jumps[i], to, prevBuffer)) return prevBuffer;
			else
			{
				// if recursive beats fails, restore backup info
				Cell beaten((max(from.row, jumps[i]->row) + min(from.row, jumps[i]->row)) / 2,
					(max(from.column, jumps[i]->column) + min(from.column, jumps[i]->column)) / 2);
				enemyCheckers.Add(beaten);
				myCheckers[*jumps[i]]->MoveChecker(from);
				for (int i = before, len = strlen(prevBuffer); i < len; prevBuffer[i++] = 0);
			}
		}
	}
	jumps.Clear();

	if (!strlen(prevBuffer))
	{
		delete [] prevBuffer;
		prevBuffer = NULL;
	}
	return NULL;
}

List Field::GetAvailableBeatJumps(Cell from)
{
	List* jumps = new List();
	if (from.isQueen())
	{
		for (byte i = 2; i < 7; i++)
		{
			if (Cell(from.row + 2, from.column + 2).IsGood())
				jumps->Add(Cell(from.row + 2, from.column + 2));
			if (Cell(from.row + 2, from.column - 2).IsGood())
				jumps->Add(Cell(from.row + 2, from.column - 2));
			if (Cell(from.row - 2, from.column + 2).IsGood())
				jumps->Add(Cell(from.row - 2, from.column + 2));
			if (Cell(from.row - 2, from.column - 2).IsGood())
				jumps->Add(Cell(from.row - 2, from.column - 2));
		}
	}
	else
	{
		jumps->Add(Cell(from.row + 2, from.column + 2));
		jumps->Add(Cell(from.row + 2, from.column - 2));
		jumps->Add(Cell(from.row - 2, from.column + 2));
		jumps->Add(Cell(from.row - 2, from.column - 2));
	}
	return *jumps;
}

byte* Field::TryBeat(Cell from, Cell to, byte* prevBuffer)
{
	// correctness checking
	if (!from.IsGood() || !to.IsGood()) return false;
	if (!from.IsBlack() || !to.IsBlack()) return false;
	if (!IsMyChecker(from) || !IsEmptyCell(to)) return false;
	byte* result = NULL;

	if ((!myCheckers[from]->isQueen()) && (abs(from.row - to.row) == 2) && (abs(from.column - to.column) == 2))
	{
		Cell beaten((max(from.row, to.row) + min(from.row, to.row)) / 2,
			(max(from.column, to.column) + min(from.column, to.column)) / 2);
		if (enemyCheckers.Contains(beaten)) result = Beat(from, to, beaten, prevBuffer);
	}
	else if ((myCheckers[from]->isQueen()) && (abs(from.row - to.row) == abs(from.column - to.column)))
	{
		Cell current, beaten;
		for (byte i = 1; i < abs(from.row - to.row); i++)
		{
			current.row = (from.row < to.row) ? from.row + i : from.row - i;
			current.column = (from.column < to.column) ? from.column + i : from.column - i;
			// only one enemy checker in a given row can be beaten
			if (enemyCheckers.Contains(current))
				if (!beaten.IsGood()) beaten = current;
				else result = NULL;
		}
		if (beaten.IsGood()) result = Beat(from, to, beaten, prevBuffer);
	}
	return result;
}

byte* Field::Beat(Cell from, Cell to, Cell beaten, byte* prevBuffer)
{
	myCheckers[from]->MoveChecker(to);
	enemyCheckers.Remove(beaten);
	byte* result = GetTurnInfo(from, to, beaten);
	memcpy(prevBuffer + strlen(prevBuffer), result, strlen(result));
	delete [] result;
	if (to.row == 0) myCheckers[to]->MakeQueen();
	return prevBuffer;
}

byte* Field::TryMove(Cell from, Cell to)
{
	if (!myCheckers[from]->isQueen() && (from.row - to.row == 1) && (abs(from.column - to.column) == 1))
	{
		if (to.row == 0) myCheckers[from]->MakeQueen();
		myCheckers[from]->MoveChecker(to);
		return GetTurnInfo(from, to);
	}
	else if (myCheckers[from]->isQueen() && (abs(from.row - to.row) == abs(from.column - to.column)))
	{
		for (byte i = 1; i < abs(from.row - to.row); i++)
		{
			byte row = (from.row < to.row) ? from.row + i : from.row - i;
			byte column = (from.column < to.column) ? from.column + i : from.column - i;
			if (!IsEmptyCell(Cell(row, column))) return NULL;
		}
		myCheckers[from]->MoveChecker(to);
		return GetTurnInfo(from, to);
	}
	else return NULL;
}

// gets turn bytes array for transferring to another player
byte* Field::GetTurnInfo(Cell from, Cell to, Cell beaten)
{
	byte* result = new byte[10];

	result[0] = '-';
	result[1] = '0' + from.row + 1;
	result[2] = '0' + from.column + 1;
	result[3] = '+';
	result[4] = '0' + to.row + 1;
	result[5] = '0' + to.column + 1;
	result[6] = 0;

	if (beaten.IsGood())
	{
		result[6] = '-';
		result[7] = '0' + beaten.row + 1;
		result[8] = '0' + beaten.column + 1;
		result[9] = 0;
	}

	return result;
}

// updates field after enemies turn
void Field::Update(byte* enemyTurn)
{
	if (!enemyTurn) return;
	for (int i = 0; i < strlen(enemyTurn); i += 3)
	{
		Cell checker('0' + 8 - enemyTurn[i + 1], '0' + 8 - enemyTurn[i + 2]);
		if (enemyTurn[i] == '+') enemyCheckers.Add(checker);
		if (enemyTurn[i] == '-')
		{
			if (myCheckers.Contains(checker)) myCheckers.Remove(checker);
			else if (enemyCheckers.Contains(checker)) enemyCheckers.Remove(checker);
		}
	}
}