typedef char byte;

// cell position class
class Cell
{

public:

	byte row;		// cell row index
	byte column;	// cell column index
	bool queen;	// is there a queen on the cell

	// does nothing
	Cell() : row(-1), column(-1), queen(false) {}

	// sets row and column to the new values
	Cell(byte row, byte column) : row(row), column(column), queen(false) {}

	// makes bad cell
	void MakeBad() { row = column = -1; queen = false; }

	// checks if cell is not bad
	bool IsGood() { return row >= 0 && column >= 0 && row < 8 && column < 8; }

	// checks if cell row and column are equal to given values
	bool Equals(byte row, byte column) const { return this->row == row && this->column == column; }
	
	// checks for cells equality
	bool Equals(Cell another) const { return Equals(another.row, another.column); }

	// checks if cell is black
	bool IsBlack() { return (row + column) % 2; }
	
	// moves checker to the new cell
	bool MoveChecker(Cell to);

	void MakeQueen() { queen = true; }

	bool isQueen() { return queen; }
};