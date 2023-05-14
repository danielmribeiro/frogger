#include "Operator.h"

int getCarsNumber(GameInfo* g) {
	int numCars = 0;

	for (int i = 0; i < g->lanes; i++)
		numCars += g->numCars[i];

	return numCars;
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
		_tprintf(_T("Cars: %d\nLevel: %d\nLanes: %d\nSpeed: %d\n"),
			getCarsNumber(buf),
			buf->level,
			buf->lanes,
			buf->speed
		);

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