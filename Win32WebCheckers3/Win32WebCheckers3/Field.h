#include "List.h"

class Field
{

protected:

	List myCheckers;	// own checkers list
	List enemyCheckers;	// enemie's checkers list

	// gets turn bytes array for transferring to another player
	byte* GetTurnInfo(Cell from, Cell to, Cell beaten = Cell(-1, -1));
	
	byte* TryBeat(Cell from, Cell to, byte* prevBuffer = NULL);

	byte* Beat(Cell from, Cell to, Cell beaten, byte* prevBuffer = NULL);

	byte* TryMove(Cell from, Cell to);

	List GetAvailableBeatJumps(Cell from);

public:

	// get cells byte representation:
	// 0 - empty, 1 - own checker, -1 - enemy checker
	byte* ToBytes();

	// set all checkers at initial positions
	bool Initialize();

	// checks if there is my checker at that cell
	bool IsMyChecker(Cell cell) const { return myCheckers.Contains(cell); }

	// checks if there is no checker at that cell
	bool IsEmptyCell(Cell cell) const { return !myCheckers.Contains(cell) && !enemyCheckers.Contains(cell); }
	
	// tries moving checker from one cell to another (maybe with beats)
	// returns: NULL if failed, otherwise byte array to be sent to another player
	// prevBuffer is an array from previous recursive calls
	byte* TryComplexMove(Cell from, Cell to, byte* prevBuffer = NULL);

	// updates field after enemies turn
	void Update(byte* enemyTurn);
};
