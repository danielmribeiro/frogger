#include "Operator.h"

int getCarsNumber(GameInfo* g) {
	int numCars = 0;

	for (int i = 0; i < g->lanes; i++)
		numCars += g->numCars[i];

	return numCars;
}

void clearConsole()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

	SetConsoleCursorPosition(hConsole, coordScreen);
}

DWORD WINAPI readGame(LPVOID p) {
	ServerData* s = (ServerData*)p;
	GameInfo* buf = (GameInfo*) getMapViewOfFile(s->hMemory);
	if (!buf) {
		_tprintf(_T("Error creating map view of shared memory\n"));
		return -1;
	}


	do {
		WaitForSingleObject(s->hMutex, INFINITE);

		// TODO print gameboard
		clearConsole();

		TCHAR gameStr[2048] = _T("");

		_tprintf(_T("FROGGER GAME\n[Level:%d] [Cars:%d] [Lanes:%d] [Speed:%d]\n"),buf->level,getCarsNumber(buf),buf->lanes,buf->speed);

		//FINISH LINE
		for (int j = 0; j < 20; j++) {
			_tprintf(_T("F"));
		}
		_tprintf(_T("\n"));
		//STREET
		for (int i = 0; i < buf->lanes; i++) {
			for (int j = 0; j < 20; j++) {
				if (buf->cars[i][0].pos.x == j) {
					_tprintf(_T("C"));
				}
				else {
					_tprintf(_T("X"));
				}
			}
			_tprintf(_T("\n"));
		}

		//START LINE
		for (int j = 0; j < 20; j++) {
			_tprintf(_T("S"));
		}

		/*TCHAR gameboardStr[] = _T("XXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX\n");
		wcscat_s(gameStr, 2048, gameboardStr);*/

		_tprintf(_T("%s\n"), gameStr);  // Print the updated string
		/*_tprintf(_T("Cars: %d\nLevel: %d\nLanes: %d\nSpeed: %d\n"),
			getCarsNumber(buf),
			buf->level,
			buf->lanes,
			buf->speed
		);*/

		ReleaseMutex(s->hMutex);
		Sleep(1000);
	} while (!s->g.exit);

	return 0;
}

void handleCommands() {
	TCHAR cmd[128];
	while (1) {
		_fgetts(cmd, 128, stdin);
		if (_tcsicmp(COMMAND_INSERT, cmd) == 0) {
			//TO DO INSERT
		}
		else if (_tcsicmp(COMMAND_STOP, cmd) == 0) {
			//TO DO STOP
		}
		else if (_tcsicmp(COMMAND_INVERT, cmd) == 0) {
			//TO DO INVERT DIRECTION
		}
		else if (_tcsicmp(COMMAND_QUIT, cmd) == 0) {
			break;
		}
		else {
			_tprintf(_T("Command not found!"));
		}
	}
}

int _tmain(int argc, TCHAR* argv[]) {
	ServerData s;
	HINSTANCE hLib = NULL;

	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	// Initialize memory DLL
	if (!initMemoryDLL(&hLib)) {
		_tprintf(_T("Error loading memory DLL!\n"));
		return -1;
	}

	if (!initServerData(&s)) {
		_tprintf(_T("Error initializing serverData! Shutting down..."));
		return -2;
	}


	if (!(s.hMemory = openSharedMemory(SERVER_MEMORY))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -3;
	}


	// Create thread to read game
	if (!createThread(&(s.hThread), readGame, &s)) {
		_tprintf(_T("Error creating game handler thread"));
		return -4;
	}

	handleCommands();

	WaitForSingleObject(s.hThread, INFINITE);

	// TODO Operator shutdown handler
	FreeLibrary(hLib);
	CloseHandle(s.hMutex);
	CloseHandle(s.hMemory);

	return 0;
}