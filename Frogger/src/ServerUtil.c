#include "ServerUtil.h"

bool isProgramUnique(HANDLE* hMutex, LPCSTR filename) {
	hMutex = CreateMutex(NULL, TRUE, filename);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return false;
	}

	return true;
}

void readArguments(ServerData* s, int size, TCHAR* args[]) {
	for (int i = 1; i < size; i++) {
		if (_tcsicmp(args[i], _T("-s")) == 0) {
			s->speed = _ttoi(args[i + 1]);
		}
		else if (_tcsicmp(args[i], _T("-l")) == 0) {
			s->lanes = _ttoi(args[i + 1]);
		}
	}
}

bool handleRegistry(ServerData* s) {
	HANDLE hKey = NULL;
	DWORD res;

	if (openRegistryKey(KEYPATH, &hKey) != ERROR_SUCCESS)
		if (createRegistryKey(KEYPATH, &hKey) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}

	if (s->speed > 0 && s->lanes > 0) {
		if (saveValueToRegistryKey(hKey, SPEED_KEY_NAME, s->speed) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
		if (saveValueToRegistryKey(hKey, LANES_KEY_NAME, s->lanes) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	if (s->speed == -1) {
		if (readValueFromRegistryKey(hKey, SPEED_KEY_NAME, &(s->speed)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	if (s->lanes == -1) {
		if (readValueFromRegistryKey(hKey, LANES_KEY_NAME, &(s->lanes)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	RegCloseKey(hKey);
	return true;
}


void handleCommands(ServerData* data) {
	TCHAR cmd[128];
	while (data->status != SHUTDOWN) {
		_fgetts(cmd, 128, stdin);

		if (_tcsicmp(COMMAND_START, cmd) == 0) {
			if (data->status != GAME_RUNNING) {
				data->status = GAME_RUNNING;
				_tprintf(_T("Data->Status: %d"), data->status);
				if (!createThread(&(data->hGameThread), handleGame, data))
					_tprintf(_T("Error creating game handler thread"));
			}
			else
				_tprintf(_T("Game is already running"));
		}
		else if (_tcsicmp(COMMAND_SUSPEND, cmd) == 0) {
			//TO DO SUSPEND GAME
		}
		else if (_tcsicmp(COMMAND_RESUME, cmd) == 0) {
			//TO DO RESUME GAME
		}
		else if (_tcsicmp(COMMAND_RESTART, cmd) == 0) {
			//TO DO RESTART GAME
		}
		else if (_tcsicmp(COMMAND_EXIT, cmd) == 0) {
			data->status = WAITING_FOR_GAME;
		}
		else if (_tcsicmp(COMMAND_QUIT, cmd) == 0) {
			data->status = SHUTDOWN;
			SetEvent(data->hEventServerShutdown);
			break;
		}
		else {
			_tprintf(_T("Command not found!"));
		}
	}
}

int getRandomValue(int max) {
	srand(time(NULL));
	return rand() % (max + 1);
}

bool isPositionEmpty(GameInfo* g, int x, int lane) {
	// Cars
	for (int i = 0; i < g->numCars[lane]; i++)
		if (g->cars[lane][i].pos.x == x)
			return false;

	// TODO Obstacules

	return true;
}

// TODO add difficulty increase due to changing level
void setGameData(ServerData* s, int level, int speed, int lanes) {
	Direction dir = RIGHT;
	GameInfo* g = &(s->g);
	g->level = level;
	g->lanes = lanes;
	g->speed = speed;
	s->status = GAME_RUNNING;
	g->isCarsRunning = true;

	// Init cars
	for (int i = 0; i < g->lanes; i++) {
		g->numCars[i] = 1; // TODO difficulty change here
		dir = getRandomValue(1);

		for (int j = 0, x = -1, y = -1; j < 8; j++) {
			do {
				x = getRandomValue(19);
				y = i;
			} while (!isPositionEmpty(g, x, i));

			g->cars[i][j].pos.x = x;
			g->cars[i][j].pos.y = y;
			g->cars[i][j].dir = dir;
		}
	}
}

void move(GameInfo* g) {
	// Move Cars
	if (g->isCarsRunning)
		for (int i = 0; i < g->lanes; i++)
			for (int j = 0; j < g->numCars[i]; j++)
				if (g->cars[i][j].dir == RIGHT)
					if (g->cars[i][j].pos.x == 19)
						g->cars[i][j].pos.x = 0;
					else
						g->cars[i][j].pos.x++;
				else if (g->cars[i][j].dir == LEFT) {
					if (g->cars[i][j].pos.x == 0)
						g->cars[i][j].pos.x = 19;
					else
						g->cars[i][j].pos.x--;
				}
}

DWORD WINAPI handleGame(LPVOID p) {
	ServerData* s = (ServerData*)p;
	int tries = 0;
	bool res = true;

	setGameData(s, 0, s->speed, s->lanes);

	// Game loop
	while (s->status == GAME_RUNNING) {
		// Move game elements
		while (WaitForSingleObject(s->hEventGameIsUpdated, 0) == WAIT_OBJECT_0
			&& s->status != SHUTDOWN
			&& s->status != WAITING_FOR_GAME);
		WaitForSingleObject(s->hMutex, INFINITE);
		move(&(s->g));

		// Save changes to shared memory
		do {
			tries++;
			res = writeSharedMemory(s->hMemory, &(s->g), sizeof(GameInfo));
		} while (!res && tries <= 3);

		// Shutdown server. Something went wrong with communication
		// TODO create event to abrundtly shutdown clients and operator
		if (!res) {
			s->status = SHUTDOWN;
			ReleaseMutex(s->hMutex);
			return -1;
		}

		SetEvent(s->hEventGameIsUpdated);
		ReleaseMutex(s->hMutex);
		// TODO add difficulty sleep multiplier
		Sleep(2000);
	}

	_tprintf(_T("Game thread is off"));

	return 0;
}

void invertCars(GameInfo* g) {
	for (int i = 0; i < g->lanes; i++)
		for (int j = 0; j < g->numCars[i]; j++)
			if (g->cars[i][j].dir == RIGHT)
				g->cars[i][j].dir = LEFT;
			else
				g->cars[i][j].dir = RIGHT;
}

DWORD WINAPI handleComms(LPVOID p) {
	ServerData* s = (ServerData*)p;
	CircularBufferMemory* circBufMemory = NULL;
	CircularBuffer buf, * pBuf = NULL;
	HANDLE* hReadSem = NULL, * hWriteSem = NULL;

	DWORD resSem = 0, resMutex;
	size_t size = sizeof(CircularBufferMemory);
	int indexWrite, indexRead;

	if (!(s->hCircBuf = createSharedMemory(SERVER_MEMORY_BUFFER,
		size))) {
		_tprintf(_T("Error creating circular buffer shared memory file! Shutting down..."));
		s->status = SHUTDOWN;
		return -1;
	}

	if (!(circBufMemory = (CircularBuffer*)getMapViewOfFile(s->hCircBuf, size))) {
		_tprintf(_T("Error creating map view of circular buffer memory file\n"));
		s->status = SHUTDOWN;
		return -2;
	}

	hReadSem = CreateSemaphore(NULL,
		0,
		BUF_SIZE,
		READ_SEMAPHORE);
	hWriteSem = CreateSemaphore(NULL,
		BUF_SIZE,
		BUF_SIZE,
		WRITE_SEMAPHORE);
	if (!hReadSem && !hWriteSem) {
		_tprintf(_T("Error creating semaphores\n"));
		s->status = 1;
		return -3;
	}

	// Initialize circular buffer
	circBufMemory->indexRead = 0;
	circBufMemory->indexWrite = 0;
	for (int i = 0; i < BUF_SIZE; i++) {
		circBufMemory->circBuf[i].type = -1;
		_tcscpy_s(circBufMemory->circBuf[i].message,
			sizeof(_T("")), _T(""));
	}

	while (s->status != SHUTDOWN) {
		do
			resSem = WaitForSingleObject(hReadSem, 1000);
		while (resSem == WAIT_TIMEOUT && s->status != SHUTDOWN);

		WaitForSingleObject(s->hMutex, 1000);

		if (s->status == SHUTDOWN) break;

		// TODO circularBuffer read handler
		indexRead = circBufMemory->indexRead++;
		pBuf = &(circBufMemory->circBuf[indexRead]);
		memcpy(&buf, pBuf, sizeof(CircularBuffer));

		if (circBufMemory->indexRead == BUF_SIZE) {
			circBufMemory->indexRead = 0;
		}

		ReleaseMutex(s->hMutex);
		ReleaseSemaphore(hWriteSem, 1, NULL);

		_tprintf(_T("Got message\nType: %d\nMsg: %s\n"),
			buf.type,
			buf.message);

		switch (buf.type) {
		case(STOP_CARS):
			s->g.isCarsRunning = !s->g.isCarsRunning;
			break;

		case(INVERT_CARS):
			invertCars(&(s->g));
			break;

		case(INSERT_OBSTACLE):
			// TODO
			_tprintf(_T("Implement insert obstacle\n"));
			break;

		default:
			break;
		}
	}

	UnmapViewOfFile(circBufMemory);

	return 0;
}

DWORD WINAPI handleClientsComms(LPVOID p) {
	ServerData* s = (ServerData*)p;
	HANDLE hServerPipe = NULL, hClientsPipe[2] = { { NULL }, {NULL} };
	DWORD clientsPIDS[2] = { 0 ,0 };
	TCHAR pipeName[50];
	DWORD pid, readPipeRes;
	int clientsConnected = 0;

	if (!(hServerPipe = CreateNamedPipe(SERVER_PIPE,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		0,
		NULL
	))) {
		_tprintf(_T("Error creating server pipe\n"));
		s->status = 1;
		return -1;
	}

	hServerPipe = CreateNamedPipe(SERVER_PIPE,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		0,
		NULL);

	while (s->status != SHUTDOWN) {
		// Wait for message from a client
		ReadFile(hServerPipe, &pid, sizeof(pid), NULL, NULL);
		readPipeRes = WaitForSingleObject(hServerPipe, 1000);

		if (readPipeRes == WAIT_TIMEOUT)
			continue;

		// Create client pipe
		switch (pid) {
		case CLIENT_CONNECTION:
			sprintf_s(pipeName, sizeof(pipeName), CLIENT_PIPE, pid);
			clientsPIDS[clientsConnected] = pid;
			hClientsPipe[clientsConnected] = CreateNamedPipe(pipeName,
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				4096,
				4096,
				0,
				NULL);
			if (!hClientsPipe[clientsConnected]) {
				_tprintf(_T("Erro a criar pipe para client"));
				pid = 1;
				WriteFile(hServerPipe, &pid, sizeof(pid), NULL, NULL);
				continue;
			}
			clientsConnected++;
			break;
		}

		pid = 0;
		WriteFile(hServerPipe, &pid, sizeof(pid), NULL, NULL);
	}

	return 0;
}