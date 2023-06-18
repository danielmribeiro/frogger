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

void printGameMap(GameInfo* g) {
	_tprintf(_T("FROGGER GAME\n[Level:%d] [Cars:%d] [Lanes:%d] [Speed:%d]\n"),
		g->level,
		getCarsNumber(g),
		g->lanes,
		g->speed);

	//FINISH LINE
	for (int j = 0; j < 20; j++)
		_tprintf(_T("F"));
	_tprintf(_T("\n"));
	//STREET
	for (int i = 0; i < g->lanes; i++) {
		for (int j = 0; j < 20; j++)
			if (g->cars[i][0].pos.x == j)
				_tprintf(_T("C"));
			else
				_tprintf(_T("X"));

		_tprintf(_T("\n"));
	}

	//START LINE
	for (int j = 0; j < 20; j++)
		_tprintf(_T("S"));
}

DWORD WINAPI readGameProc(LPVOID p) {
	ServerData* s = (ServerData*)p;
	GameInfo* pGameInfo = NULL, localGameInfo;
	if (!(pGameInfo = (GameInfo*)getMapViewOfFile(s->hMemory,
		sizeof(GameInfo)))) {
		_tprintf(_T("Error creating map view of shared memory\n"));
		return -1;
	}

	while (s->status != SHUTDOWN) {
		// Read game data from server
		while (WaitForSingleObject(s->hEventGameIsUpdated, 0) && s->status != SHUTDOWN);
		if (s->status == SHUTDOWN) return 1;

		WaitForSingleObject(s->hMutex, INFINITE);
		memcpy(&localGameInfo, pGameInfo, sizeof(GameInfo));
		ReleaseMutex(s->hMutex);
		// ResetEvent(s->hEventGameIsUpdated);

		// print game map
		clearConsole();
		printGameMap(&localGameInfo);
	}

	return 0;
}

DWORD WINAPI commandReaderProc(LPVOID* p) {
	CommandReaderData* o = (CommandReaderData*)p;
	TCHAR command[128];

	while (o->status != SHUTDOWN) {
		while (WaitForSingleObject(o->hEventHasCommand, 0) == WAIT_OBJECT_0 && o->status != SHUTDOWN);
		if (o->status == SHUTDOWN)
			break;

		_tprintf(_T("> "));
		_fgetts(command, 128, stdin);

		SetEvent(*(o->hEventHasCommand));
		WaitForSingleObject(o->hCommandMutex, INFINITE);
		memcpy(o->command, command, sizeof(command));
		ReleaseMutex(o->hCommandMutex);
	}

	return 0;
}

DWORD WINAPI commandsHandlerProc(LPVOID* p) {
	CommandReaderData crd;
	ServerData* s = (ServerData*)p;
	CircularBufferMemory* circBufMemory;
	CircularBuffer buf, * pBuf = NULL;
	HANDLE hReadSem,
		hWriteSem,
		hCommandReaderThread,
		hCommandMutex,
		hEventCommandUpdated;
	TCHAR command[128], eventName[50];
	DWORD resSem = 0;

	_tprintf(_T("Command Handler proc start\n"));


	if (!(s->hCircBuf = openSharedMemory(SERVER_MEMORY_BUFFER))) {
		_tprintf(_T("Error opening circular buffer shared memory file! Shutting down..."));
		s->status = 1;
		return -1;
	}

	if (!(circBufMemory = getMapViewOfFile(s->hCircBuf, sizeof(CircularBufferMemory)))) {
		_tprintf(_T("Error creating map view of circular buffer memory file\n"));
		s->status = 1;
		return -2;
	}

	if (!(hReadSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS,
		FALSE,
		READ_SEMAPHORE))) {
		_tprintf(_T("Error creating read semaphore\n"));
		s->status = SHUTDOWN;
		return -3;
	}

	if (!(hWriteSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS,
		FALSE,
		WRITE_SEMAPHORE))) {
		_tprintf(_T("Error creating write semaphore\n"));
		s->status = SHUTDOWN;
		return -4;
	}

	// TODO create command reader mutex
	if (!(hCommandMutex = CreateMutex(NULL, FALSE, OPERATOR_COMMAND_MUTEX))) {
		s->status = SHUTDOWN;
		return -5;
	}

	// TODO create command reader event
	swprintf(eventName, sizeof(eventName), EVENT_COMMAND_UPDATED, GetCurrentProcessId());
	if (!(hEventCommandUpdated = CreateEvent(NULL, TRUE, FALSE, eventName))) {
		_tprintf(_T("Error creating game update event. Shutting down"));
		return -6;
	}

	// TODO initialize command reader data
	crd.command = &command;
	crd.hCommandMutex = &hCommandMutex;
	crd.hEventHasCommand = &hEventCommandUpdated;

	// TODO create command reader thread
	if (!createThread(&hCommandReaderThread, commandReaderProc, &crd)) {
		_tprintf(_T("Error creating command reader thread\n"));
		return -7;
	}

	// Initialize local buffer
	buf.type = -1;
	_tcscpy_s(buf.message,
		sizeof(_T("")), _T(""));

	while (s->status != SHUTDOWN) {
		while (WaitForSingleObject(hEventCommandUpdated, 0) && s->status != SHUTDOWN);
		WaitForSingleObject(hCommandMutex, INFINITE);
		memcpy(command, crd.command, sizeof(command));

		if (_tcsicmp(COMMAND_INSERT, command) == 0) {
			//TO DO INSERT
		}
		else if (_tcsicmp(COMMAND_STOP, command) == 0) {
			buf.type = STOP_CARS;
		}
		else if (_tcsicmp(COMMAND_INVERT, command) == 0) {
			buf.type = INVERT_CARS;
		}
		else if (_tcsicmp(COMMAND_QUIT, command) == 0) {
			s->status = SHUTDOWN;
		}
		else {
			_tprintf(_T("\nCommand not found!\n"));
			ResetEvent(hEventCommandUpdated);
			ReleaseMutex(hCommandMutex);
			continue;
		}

		ResetEvent(hEventCommandUpdated);
		ReleaseMutex(hCommandMutex);
		while (WaitForSingleObject(hWriteSem, 0) == WAIT_TIMEOUT && s->status != SHUTDOWN);
		if (s->status == SHUTDOWN)
			break;

		// Copy to circular buffer memory
		pBuf = &(circBufMemory->circBuf[circBufMemory->indexWrite]);
		memcpy(pBuf, &buf, sizeof(CircularBuffer));
		circBufMemory->indexWrite++;

		if (circBufMemory->indexWrite == BUF_SIZE)
			circBufMemory->indexWrite = 0;

		ReleaseSemaphore(hReadSem, 1, NULL);
	}
	_tprintf(_T("Command Handler proc exit\n"));
	closeSharedMemory(s->hCircBuf, circBufMemory);
	return 0;
}

DWORD WINAPI watchForShutdownEvent(LPVOID* p) {
	ServerData* s = (ServerData*)p;

	WaitForSingleObject(s->hEventServerShutdown, INFINITE);
	s->status = SHUTDOWN;
	if (WaitForSingleObject(s->hClientsComms, 2000) != WAIT_OBJECT_0)
		if (!TerminateThread(s->hClientsComms, 1))
			return -7;

	return 0;
}

int _tmain(int argc, TCHAR* argv[]) {
	ServerData s;
	HINSTANCE hLib = NULL;
	HANDLE hServerShutdown;

	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	// Initialize memory DLL
	if (!initMemoryDLL(&hLib)) {
		_tprintf(_T("Error loading memory DLL!\n"));
		return -1;
	}

	if (!initServerData(&s, false)) {
		_tprintf(_T("Error initializing serverData! Shutting down...\n"));
		return -2;
	}

	if (!(s.hMemory = openSharedMemory(SERVER_MEMORY))) {
		_tprintf(_T("Error open shared memory file! Shutting down...\n"));
		return -3;
	}

	// Create thread to read game
	if (!createThread(&(s.hGameThread), readGameProc, &s)) {
		_tprintf(_T("Error creating game handler thread\n"));
		return -4;
	}

	// Create thread to check event for shutdown
	if (!createThread(&(hServerShutdown), watchForShutdownEvent, &s)) {
		_tprintf(_T("Error creating shutdown event thread\n"));
		return -5;
	}

	// Create thread to handle commands
	if (!createThread(&(s.hClientsComms), commandsHandlerProc, &s)) {
		_tprintf(_T("Error creating handler commands thread"));
		return -6;
	}

	WaitForSingleObject(s.hGameThread, INFINITE);
	WaitForSingleObject(s.hClientsComms, INFINITE);
	WaitForSingleObject(hServerShutdown, INFINITE);
	WaitForSingleObject(watchForShutdownEvent, INFINITE);

	// TODO Operator shutdown handler
	FreeLibrary(hLib);
	CloseHandle(s.hMutex);
	CloseHandle(s.hMemory);

	return 0;
}