#include "stdafx.h"
#include "UserInterface.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	Game game;
	UserInterface ui(game);
	return ui.Show(hInstance, nCmdShow);
}
