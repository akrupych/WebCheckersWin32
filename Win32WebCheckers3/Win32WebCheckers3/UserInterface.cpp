#include "stdafx.h"
#include "Resource.h"
#include "UserInterface.h"

HINSTANCE UserInterface::hInst;				// current instance
Game UserInterface::game;					// current game
Cell UserInterface::selected;			// row and column indices of the selected checker

// set game for ui processing
UserInterface::UserInterface(Game game)
{
	this->game = game;
	selected.MakeBad();
}

// register window
ATOM UserInterface::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32WEBCHECKERS3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32WEBCHECKERS3);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// initialize and show window
bool UserInterface::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 438, 478, NULL, NULL, hInstance, NULL);

	if (!hWnd) return false;
	
	if (!game.SeedGame()) MessageBoxA(hWnd, (LPCSTR)"Can't start server", (LPCSTR)"Error", MB_OK);
	else SetTimer(hWnd, 0, 1000, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}

int CALLBACK UserInterface::ProcessConnectDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	char* playerIP = new char[100];

	switch (message)
	{
		case WM_COMMAND: 
			switch (LOWORD(wParam)) 
			{ 
			case IDC_BUTTON1:
				GetDlgItemTextA(hwnd, IDC_EDIT1, playerIP, 100);
				if (game.ConnectTo(playerIP))
				{
					KillTimer(GetParent(hwnd), 0);
					SetTimer(GetParent(hwnd), 1, 1000, NULL);
					InvalidateRect(GetParent(hwnd), NULL, true);
				}
				else MessageBoxA(hwnd, playerIP, "Can't connect to", MB_OK);
				DestroyWindow(hwnd);
				break;
			}
	}

	delete [] playerIP;
	return 0;
}

// get and process window messages
LRESULT CALLBACK UserInterface::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HWND connectionDialog;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_FILE_CONNECT:
			connectionDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, (DLGPROC)ProcessConnectDialog);
			ShowWindow(connectionDialog, SW_SHOW);
			InvalidateRect(hWnd, NULL, true);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawField(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		DragChecker(hWnd);
		break;
	case WM_MOUSEMOVE:
		if (selected.IsGood())
			InvalidateRect(hWnd, NULL, true);
			//ShowMove(hWnd);
		break;
	case WM_LBUTTONUP:
		DropChecker(hWnd);
		break;
	case WM_TIMER:
		if (LOWORD(wParam) == 0) CheckForConnected(hWnd);
		else if (LOWORD(wParam) == 1) CheckForNextTurn(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// about box message handler
INT_PTR CALLBACK UserInterface::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// show main window
int UserInterface::Show(HINSTANCE hInstance, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32WEBCHECKERS3, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return 0;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32WEBCHECKERS3));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


// checks for income connection
void UserInterface::CheckForConnected(HWND hwnd)
{
	if (!game.Start()) return;
	KillTimer(hwnd, 0);
	//MessageBoxA(hwnd, "Your turn!", "Game started", MB_OK);
	InvalidateRect(hwnd, NULL, true);
}

// checks for the next turn beginning
void UserInterface::CheckForNextTurn(HWND hwnd)
{
	if (!game.GetTurnFromEnemy()) return;
	KillTimer(hwnd, 1);
	//MessageBoxA(hwnd, "Your time is now!", "Shoot", MB_OK);
	InvalidateRect(hwnd, NULL, true);
}

// draw game field with checkers
void UserInterface::DrawField(HWND hwnd, HDC hdc)
{
	HBRUSH brush;
	int x, y;
	byte* field = game.GetField();

	// draw field with standing checkers
	for (int i = 0, x = fieldBeginX; i < 8; i++, x += cellSize)
	{
		for (int j = 0, y = fieldBeginY; j < 8; j++, y += cellSize)
		{
			brush = CreateSolidBrush((i + j) % 2 ? RGB(0, 0, 0) : RGB(255, 255, 255));
			SelectObject(hdc, brush);
			Rectangle(hdc, x, y, x + cellSize, y + cellSize);
			DeleteObject(brush);

			byte current = field[8 * j + i];
			if (selected.Equals(j, i)) current = 0;
			if (current)	// draw checker
			{
				brush = CreateSolidBrush(current < 0 ? RGB(255, 0, 0) : RGB(0, 0, 255));
				SelectObject(hdc, brush);
				Ellipse(hdc, x, y, x + cellSize, y + cellSize);
				DeleteObject(brush);
				if (abs(current) % 2 == 0)	// mark queen
				{
					brush = CreateSolidBrush(RGB(255, 255, 255));
					SelectObject(hdc, brush);
					Ellipse(hdc, x + cellSize / 4, y + cellSize / 4,
						x + 3 * cellSize / 4, y + 3 * cellSize / 4);
					DeleteObject(brush);
				}
			}
		}
	}
	delete [] field;
	// draw non-standing checker
	if (selected.IsGood()) DrawMovingChecker(hwnd, hdc);
}

// draws dragged and currently moving checker
void UserInterface::DrawMovingChecker(HWND hwnd, HDC hdc)
{
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(hwnd, &cursor);
	cursor.x -= (cellSize / 2);
	cursor.y -= (cellSize / 2);

	HBRUSH brush = CreateSolidBrush(RGB(125, 0, 255));
	SelectObject(hdc, brush);

	Ellipse(hdc, cursor.x, cursor.y, cursor.x + cellSize, cursor.y + cellSize);
	DeleteObject(brush);
}

// screen location to field location
// (-1, -1) if outside of the field
Cell UserInterface::OnField(HWND hwnd, POINT point)
{
	ScreenToClient(hwnd, &point);
	point.x -= fieldBeginX;
	point.y -= fieldBeginY;
	if (!((point.x >= 0) && (point.x < cellSize * 8) && (point.y >= 0) && (point.y < cellSize * 8)))
		return Cell(-1, -1);
	return Cell(point.y / cellSize, point.x / cellSize);
}

// drags own checker from field
void UserInterface::DragChecker(HWND hwnd)
{
	if (!game.IsMyTurn()) return;

	POINT cursor;
	GetCursorPos(&cursor);

	selected = OnField(hwnd, cursor);

	byte* field = game.GetField();
	if (field[8 * selected.row + selected.column] <= 0)
		selected.MakeBad();

	InvalidateRect(hwnd, NULL, true);
}

// drops checker on field
void UserInterface::DropChecker(HWND hwnd)
{
	if (!game.IsMyTurn()) return;

	POINT cursor;
	GetCursorPos(&cursor);

	Cell newCell = OnField(hwnd, cursor);
	if (game.MakeTurn(selected, newCell))
		SetTimer(hwnd, 1, 1000, NULL);

	InvalidateRect(hwnd, NULL, true);
	selected.MakeBad();
}

/*void ShowMove(HWND hwnd)
{
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(hwnd, &cursor);

	RECT rect;
	rect.left = cursor.x - (cellSize / 2);
	rect.right = cursor.x + (cellSize / 2);
	rect.top = cursor.y - (cellSize / 2);
	rect.bottom = cursor.y + (cellSize / 2);

	InvalidateRect(hwnd, &rect, false);
}*/