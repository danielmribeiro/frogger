#include "ServerUtil.h"

HANDLE hEventOver;

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

		if (_tcsicmp(COMMAND_SUSPEND, cmd) == 0) {
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

	// TODO init obstacles

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
		
		// TODO add write to all clients or just one 
		// if it's individual game
		/*for (DWORD i = 0; i < 1; i++) {
			WriteFile(s->clientPipes[i].hPipe, &(s->g), sizeof(GameInfo), NULL, NULL);
		}*/
		WriteFile(s->clientPipes[0].hPipe, &(s->g), sizeof(GameInfo), NULL, NULL);

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

ClientRequest getClientRequest(HANDLE hPipe) {
	ClientRequest clientRequest;
	DWORD dwBytesRead;
	bool readSuccess = false;

	clientRequest.type = -1;
	readSuccess = ReadFile(hPipe, &clientRequest, sizeof(ClientRequest), &dwBytesRead, NULL);
	if (readSuccess == false)
		_tprintf(_T("Error reading client pipe\n"));

	return clientRequest;
}

void handleClientRequest(HANDLE hPipe, ClientRequest c, ServerData* s) {
	DWORD res = 1;

	switch (c.type) {
	case CLIENT_CONNECT:
		_tprintf(_T("Client has connected!\n"));
		break;
	case PLAY_INDIVIDUAL:
		if (s->status != GAME_RUNNING) {
			s->status = GAME_RUNNING;
			_tprintf(_T("res = %d\n"), res);
			WriteFile(hPipe, &res, sizeof(res), NULL, NULL);

			if (!createThread(&(s->hGameThread), handleGame, s)) {
				_tprintf(_T("Error creating game handler thread\n"));
				res = 0;
			}
				
			// Confirm that client can start
		}
		break;
	case PLAY_COMPETITIVE:
		break;
	}
}

DWORD WINAPI pipeHandlerProc(LPVOID p) {
	ClientRequest clientRequest = { 0 };
	ClientPipe* c = (ClientPipe*)p;

	while (c->s->status != SHUTDOWN) {
		clientRequest = getClientRequest(c->hPipe, &clientRequest);
		if (clientRequest.type == CLIENT_SHUTDOWN)
			break;

		_tprintf(_T("ClientRequest: Type %d\n"), clientRequest.type);
		handleClientRequest(c->hPipe, clientRequest, c->s);
	}

	_tprintf(_T("Client %d has disconnected\n"), c->playerID);
	c->isActive = FALSE;
	DisconnectNamedPipe(c->hPipe);
	return 0;
}

DWORD getFirstClientPipeAvailable(ClientPipe* c) {
	for (DWORD i = 0; i < MAX_PLAYERS; i++)
		if (c[i].isActive == FALSE)
			return i;
	return -1;
}

DWORD WINAPI handleClientsComms(LPVOID p) {
	ServerData* s = (ServerData*)p;
	ClientPipe clientPipe[MAX_PLAYERS];
	s->clientPipes = &clientPipe;

	OVERLAPPED stOverlapped;
	HANDLE pipeConnections[MAX_PLAYERS];
	HANDLE clientConnectionThreads[MAX_PLAYERS];
	DWORD clientConnectionThreadID[MAX_PLAYERS];
	HANDLE hPipeTmp;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		pipeConnections[i] = INVALID_HANDLE_VALUE;
		clientConnectionThreads[i] = INVALID_HANDLE_VALUE;
		clientPipe[i].hPipe = INVALID_HANDLE_VALUE;
		clientPipe[i].s = s;
		clientPipe[i].playerID = i + 1;
		clientPipe[i].isActive = FALSE;
	}

	hEventOver = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEventOver == NULL) {
		_tprintf(_T("Error creating event for pipes\n"));
		s->status = SHUTDOWN;
		return -1;
	}

	stOverlapped.hEvent = hEventOver;

	while (s->status != SHUTDOWN) {
		DWORD index = getFirstClientPipeAvailable(clientPipe);
		if (index == -1) continue;

		// TODO verify which clients has disconnected
		hPipeTmp = CreateNamedPipe(SERVER_PIPE,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			4096,
			4096,
			0,
			NULL);
		if (hPipeTmp == INVALID_HANDLE_VALUE) {
			_tprintf(_T("Error creating server pipe\n"));
			break;
		}

		ConnectNamedPipe(hPipeTmp, &stOverlapped);
		WaitForSingleObject(hEventOver, INFINITE);

		if (s->status == SHUTDOWN)
			break;

		clientPipe[index].hPipe = hPipeTmp;
		clientPipe[index].isActive = TRUE;
		clientConnectionThreads[index] = CreateThread(
			NULL,
			0,
			pipeHandlerProc,
			&clientPipe[index],
			0,
			&clientConnectionThreadID[index]
		);

		pipeConnections[index] = hPipeTmp;
	}

	WaitForMultipleObjects(MAX_PLAYERS, clientConnectionThreads, TRUE, INFINITE);

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (clientConnectionThreads[i] != INVALID_HANDLE_VALUE) {
			DisconnectNamedPipe(pipeConnections[i]);
			CloseHandle(pipeConnections[i]);
		}
	}

	return 0;



	return 0;
}
