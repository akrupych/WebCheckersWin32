#include "stdafx.h"
#include "Game.h"

// main window class for user interface
class UserInterface
{

protected:

	static Game game;						// current game
	static const int cellSize = 50;			// cell square with and height
	static const int fieldBeginX = 10;		// left field margin
	static const int fieldBeginY = 10;		// top field margin
	static Cell selected;					// row and column indices of the selected checker

	#define MAX_LOADSTRING 100				// max loadstring size

	static HINSTANCE hInst;					// current instance
	TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];	// the main window class name

	// registers window
	ATOM MyRegisterClass(HINSTANCE hInstance);
	
	// initializes and shows window
	bool InitInstance(HINSTANCE hInstance, int nCmdShow);

	// gets and processes window messages
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// gets and processes connect dialog messages
	static int CALLBACK ProcessConnectDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// about box message handler
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	// draws game field with checkers
	static void DrawField(HWND hwnd, HDC hdc);

	// screen location to field location
	// (-1, -1) if outside of the field
	static Cell OnField(HWND hwnd, POINT point);
	
	// drags own checker from field
	static void DragChecker(HWND hwnd);

	// draws dragged and currently moving checker
	static void DrawMovingChecker(HWND hwnd, HDC hdc);

	// drops checker on field
	static void DropChecker(HWND hwnd);

	// checks for income connection
	static void CheckForConnected(HWND hwnd);

	// checks for the next turn beginning
	static void CheckForNextTurn(HWND hwnd);

public:

	// set game for ui processing
	UserInterface(Game game);

	// show main window
	int Show(HINSTANCE hInstance, int nCmdShow);
};