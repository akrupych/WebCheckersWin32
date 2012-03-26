#include "stdafx.h"
#include "Cell.h"

class List
{

protected:

	// one-linked node for CheckersList element
	struct Node
	{
		Cell value;
		Node* next;

		// sets value and nulls next address
		Node (Cell value) : value(value), next(NULL) {}

		// copy constructor
		//Node (Node& another);
	}
	*begin; // first CheckersList element

public:

	// sets first element to NULL
	List () : begin(NULL) {}

	// adds "length" arguments
	/*CheckersList (unsigned length, ... );

	// copy constructor
	CheckersList (const CheckersList& another) : begin (new Node(*another.begin)) {}*/

	// adds "count" of "element"
	bool Add (Cell element, unsigned count = 1);

	// checks if list contains equal element
	bool Contains(Cell cell) const;

	// inserts element at defined position (0-based and <= CheckersList's size)
	bool Insert (Cell element, unsigned position);

	// removes element equals given
	bool Remove (Cell cell);

	// removes element at position (0-based and < CheckersList's size)
	bool Remove (unsigned position);

	// removes elements from "begin" to "end" position inclusively
	bool Remove (unsigned begin, unsigned end);

	// removes all elements from CheckersList
	void Clear ();

	// checker at index
	Cell* operator [] (unsigned position);
	
	// checker at cell
	Cell* operator [] (Cell cell);

	// returns elements count
	int Count () const;

	// destructor
	~List () { Clear(); }
};