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

bool initMemoryDLL(HINSTANCE* h) {
	if (!(h = LoadLibrary(_T("Memory.dll"))))
		return false;

	return true;
}

bool initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->hMutexStop = NULL;
	s->speed = -1;
	s->lanes = -1;
	s->gamemode = -1;

	if (!(s->hMutexStop = CreateMutex(NULL, FALSE, SERVER_GAME_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	if (!(s->hMutex = CreateMutex(NULL, FALSE, SERVER_MEMORY_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	return true;
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

bool createThread(HANDLE* h, LPTHREAD_START_ROUTINE f, LPVOID ptrData) {
	*h = CreateThread(NULL,
		0,
		f,
		ptrData,
		0,
		NULL);

	if (*h == NULL) return false;
	return true;
}

void handleCommands(ServerData* data) {
	TCHAR cmd[128];
	while (1) {
		_fgetts(cmd, 128, stdin);
		if (_tcsicmp(COMMAND_DEMO, cmd) == 0) {
			if (data->status == 1) {
				data->gamemode = GAME_DEMO;
				if (!createThread(data->hThread, handleGame, data)) {
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
	GameInfo* g = &(s->g);
	g->level = level;
	g->lanes = lanes;
	g->speed = speed;
	s->status = 0;

	// Init cars
	for (int i = 0; i < g->lanes; i++) {
		g->numCars[i] = 1; // TODO difficulty change here

		for (int j = 0, x = -1, y = -1; j < 8; j++) {
			do {
				x = getRandomValue(19);
				y = i;
			} while (!isPositionEmpty(g, x, i));

			g->cars[i][j].pos.x = x;
			g->cars[i][j].pos.y = y;
			g->cars[i][j].dir = getRandomValue(1);
		}
	}

	_tprintf(_T("Belele"));
}

void move(GameInfo* g) {
	// Cars
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
	setGameData(s, 0, s->speed, s->lanes);

	// Game loop
	while (!s->status &&
		WaitForSingleObject(s->hMutexStop, INFINITE) == WAIT_OBJECT_0) {
		// Move game elements
		move(&(s->g));

		for (int i = 0; i < MAX_LANES; i++)
			for (int j = 0; j < s->g.numCars[i]; j++)
				_tprintf(_T("Lane %d: Car(%d|%d)\n"),
					i,
					s->g.cars[i][j].pos.x,
					s->g.cars[i][j].pos.y);

		ReleaseMutex(s->hMutexStop);
		Sleep(1000);

	}
}