#include "stdafx.h"
#include "Cell.h"

// moves checker to the new cell
bool Cell::MoveChecker(Cell to)
{
	if (!IsBlack() || !IsGood()) return false;
	row = to.row;
	column = to.column;
}