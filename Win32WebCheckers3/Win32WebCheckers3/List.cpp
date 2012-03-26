#include "stdafx.h"
#include <iostream>
#include <cstdarg>
#include "List.h"

// copy constructor
/*CheckersList::Node::Node (Node& another)
{
	if (&another != NULL)
	{
		value = new Checker(*another.value);
		if (another.next != NULL) next = new Node(*another.next);
		else next = NULL;
	}
}

// adds "length" arguments
CheckersList::CheckersList (unsigned length, ... ) : begin(NULL)
{
	va_list args;
	va_start(args, length);
	while (length)
	{
		add(va_arg(args, Checker*));
		length--;
	}
}*/

// adds "count" of "element"
bool List::Add (Cell element, unsigned count)
{
	while (count)
	{
		//if (!add(element)) return false;
		if (!begin)
		{
			begin = new Node(element);	// if CheckersList is empty
			if (!begin) return false;
		}
		else
		{
			// move to last element and add new node
			Node* current = begin;
			while (current->next) current = current->next;
			current->next = new Node(element);
			if (!current->next) return false;
		}
		count--;
	}
	return true;
}

// checks if list contains equal element
bool List::Contains(Cell cell) const
{
	for (Node* current = begin; current; current = current->next)
		if (current->value.Equals(cell))
			return true;
	return false;
}

// inserts element at defined position (0-based and <= CheckersList's size)
bool List::Insert (Cell element, unsigned position)
{
	// element doesn't exist (adding to end allowed)
	if (position > Count())
	{
		std::cerr << "Insert position parameter must be less or equal with CheckersList size\n";
		return false;
	}

	// element can be inserted
	Node* new_node = new Node(element);
	if (!new_node) return false;

	// change begin if position == 0
	if (!position)
	{
		new_node->next = begin;
		begin = new_node;
		return true;
	}

	// find previous node to reset it's next
	Node* previous = begin;
	for (; position > 1; position--) previous = previous->next;

	// insert and link nodes
	new_node->next = previous->next;
	previous->next = new_node;

	return true;
}

// removes element equals given
bool List::Remove (Cell cell)
{
	int position = 0;
	for (Node* current = begin; current; current = current->next, position++)
		if (current->value.Equals(cell))
			return Remove(position);
	return false;
}

// removes element at position (0-based and < CheckersList's size)
bool List::Remove (unsigned position)
{
	// element doesn't exist
	if (position >= Count())
	{
		std::cerr << "Remove position parameter must be less than CheckersList size\n";
		return false;
	}

	// change begin if position == 0
	if (position == 0)
	{
		Node* to_remove = begin;
		begin = begin->next;
		delete to_remove;
		return true;
	}

	// find previous node to reset it's next
	Node* previous = begin;
	for (; position > 1; position--) previous = previous->next;

	// remove and link nodes
	Node* to_remove = previous->next;
	previous->next = to_remove->next;
	delete to_remove;
	return true;
}

// removes elements from "begin" to "end" position inclusively
bool List::Remove (unsigned begin, unsigned end)
{
	if (begin > end)
	{
		std::cerr << "Begin parameter must be less or equal to end parameter\n";
		return false;
	}
	if (begin >= Count() || end >= Count())
	{
		std::cerr << "Begin and end parameters must be in the CheckersList's elements range\n";
		return false;
	}
	for (register unsigned i = begin; i <= end; i++) Remove(begin);
}

// removes all elements from CheckersList
void List::Clear ()
{
	while (begin)
	{
		Node* to_remove = begin;
		begin = begin->next;
		delete to_remove;
	}
	delete begin;
	begin = NULL;
}

// checker at index
Cell* List::operator [] (unsigned position)
{
	Node* current = begin;
	while (position)
	{
		current = current->next;
		position--;
	}
	return &current->value;
}

// checker at cell
Cell* List::operator [] (Cell cell)
{
	for (Node* current = begin; current; current = current->next)
		if (current->value.Equals(cell))
			return &current->value;
	return NULL;
}

// returns elements count
int List::Count () const
{
	if (!begin) return 0; // for empty CheckersList
	int result = 1;
	for (Node* current = begin; current->next; current = current->next, result++);
	return result;
}