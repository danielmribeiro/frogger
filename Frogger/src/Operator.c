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
	GameInfo* pGameInfo = (GameInfo*)getMapViewOfFile(s->hMemory,
		sizeof(GameInfo));
	if (!pGameInfo) {
		_tprintf(_T("Error creating map view of shared memory\n"));
		return -1;
	}

	while (!s->g.exit) {
		WaitForSingleObject(s->hMutex, INFINITE);

		// TODO print gameboard
		clearConsole();

		TCHAR gameStr[2048] = _T("");

		_tprintf(_T("FROGGER GAME\n[Level:%d] [Cars:%d] [Lanes:%d] [Speed:%d]\n"),
			pGameInfo->level,
			getCarsNumber(pGameInfo),
			pGameInfo->lanes,
			pGameInfo->speed);

		//FINISH LINE
		for (int j = 0; j < 20; j++) {
			_tprintf(_T("F"));
		}
		_tprintf(_T("\n"));
		//STREET
		for (int i = 0; i < pGameInfo->lanes; i++) {
			for (int j = 0; j < 20; j++) {
				if (pGameInfo->cars[i][0].pos.x == j) {
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

		_tprintf(_T("%s\n"), gameStr);

		ReleaseMutex(s->hMutex);
		Sleep(1000);
	}

	return 0;
}

void handleCommands(ServerData* s) {
	CircularBufferMemory* circBufMemory = NULL;
	CircularBuffer buf, * pBuf = NULL;
	HANDLE* hReadSem = NULL, hWriteSem = NULL;
	TCHAR cmd[128];
	DWORD resSem = 0;
	size_t size = sizeof(CircularBufferMemory);
	bool exit = false;
	int indexWrite = 0;

	if (!(s->hCircBuf = openSharedMemory(SERVER_MEMORY_BUFFER, size))) {
		_tprintf(_T("Error opening circular buffer shared memory file! Shutting down..."));
			s->status = 1;
			s->g.exit = 1;
			return -1;
	}

	if (!(circBufMemory = getMapViewOfFile(s->hCircBuf, size))) {
		_tprintf(_T("Error creating map view of circular buffer memory file\n"));
		s->status = 1;
		s->g.exit = 1;
		return -2;
	}

	hReadSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS,
		FALSE, 
		READ_SEMAPHORE);
	hWriteSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS,
		FALSE, 
		WRITE_SEMAPHORE);
	if (!hReadSem && !hWriteSem) {
		_tprintf(_T("Error creating semaphores\n"));
		s->status = 1;
		s->g.exit = 1;
		return -3;
	}

	buf.type = -1;
	_tcscpy_s(buf.message,
		sizeof(_T("")), _T(""));

	while (!exit) {
		_fgetts(cmd, 128, stdin);
		
		if (_tcsicmp(COMMAND_INSERT, cmd) == 0) {
			//TO DO INSERT
		}
		else if (_tcsicmp(COMMAND_STOP, cmd) == 0) {
			buf.type = STOP_CARS;
		}
		else if (_tcsicmp(COMMAND_INVERT, cmd) == 0) {
			buf.type = INVERT_CARS;
		}
		else if (_tcsicmp(COMMAND_QUIT, cmd) == 0) {
			exit = true;
			s->g.exit = true;
		}
		else {
			_tprintf(_T("Command not found!"));
			continue;
		}

		do {
			resSem = WaitForSingleObject(hWriteSem, 1000);
		} while (resSem == WAIT_TIMEOUT && !s->g.exit);

		indexWrite = circBufMemory->indexWrite;
		pBuf = &(circBufMemory->circBuf[indexWrite]);

		// Copy to circular buffer memory
		memcpy(pBuf, &buf,sizeof(CircularBuffer));
		circBufMemory->indexWrite++;

		if (circBufMemory->indexWrite == BUF_SIZE) {
			circBufMemory->indexWrite = 0;
		}

		ReleaseSemaphore(hReadSem, 1, NULL);
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
	if (!createThread(&(s.hGameThread), readGame, &s)) {
		_tprintf(_T("Error creating game handler thread"));
		return -4;
	}

	handleCommands(&s);

	WaitForSingleObject(s.hGameThread, INFINITE);

	// TODO Operator shutdown handler
	FreeLibrary(hLib);
	CloseHandle(s.hMutex);
	CloseHandle(s.hMemory);

	return 0;
}