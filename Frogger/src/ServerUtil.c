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
	while (!data->status) {
		_fgetts(cmd, 128, stdin);
		if (_tcsicmp(COMMAND_DEMO, cmd) == 0) {
			if (data->status == 1) {
				data->gamemode = GAME_DEMO;
				if (!createThread(data->hGameThread, handleGame, data)) {
					_tprintf(_T("Error creating game handler thread"));
					return -1;
				}
			}
			else {
				_tprintf(_T("Game is already runnning"));
			}
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
			//TO DO EXIT GAME
		}
		else if (_tcsicmp(COMMAND_QUIT, cmd) == 0) {

			break;//temporary

			//send warning to other apps
			//close_serverapp(ERROR_SUCCESS_COMMAND_QUIT, data);
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
	s->status = 0;
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
	while (!s->status) {
		// Move game elements
		WaitForSingleObject(s->hMutex, INFINITE);
		move(&(s->g));

		// Save changes to shared memory
		do {
			tries++;
			res = writeSharedMemory(s->hMemory, &(s->g), sizeof(GameInfo));
		} while (!res && tries <= 3);

		// Shutdown server. Something went wrong with communication
		if (!res) {
			ReleaseMutex(s->hMutex);
			return -1;
		}

		ReleaseMutex(s->hMutex);
		Sleep(2000);
	}

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
		s->status = 1;
		return -1;
	}

	if (!(circBufMemory = (CircularBuffer*)getMapViewOfFile(s->hCircBuf, size))) {
		_tprintf(_T("Error creating map view of circular buffer memory file\n"));
		s->status = 1;
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

	while (!s->status) {
		do {
			resSem = WaitForSingleObject(hReadSem, 1000);

		} while (resSem == WAIT_TIMEOUT && !s->status);

		WaitForSingleObject(s->hMutex, 1000);

		if (s->status) break;

		// TODO circularBuffer read handler
		indexRead = circBufMemory->indexRead++;
		pBuf = &(circBufMemory->circBuf[indexRead]);
		memcpy(&buf, pBuf, sizeof(CircularBuffer));

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

		if (circBufMemory->indexRead == BUF_SIZE) {
			circBufMemory->indexRead = 0;
		}

		ReleaseMutex(s->hMutex);
		ReleaseSemaphore(hWriteSem, 1, NULL);
	}

	UnmapViewOfFile(circBufMemory);

	return 0;
}