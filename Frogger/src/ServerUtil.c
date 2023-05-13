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

void initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->speed = -1;
	s->lanes = -1;
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

bool createThread(HANDLE* h, DWORD WINAPI f, LPVOID ptrData) {
	h = CreateThread(NULL,
		0,
		f,
		ptrData,
		0,
		NULL);

	if (!h) return false;
	return true;
}

// TODO add difficulty increase due to changing level
void setGameData(GameInfo* g, int level, int speed, int lanes) {
	g->level = level;
	g->lanes = lanes;
	g->speed = speed;

	// Init cars
	for (int i = 0; i < 10; i++) {
		g->numCars[i] = 0;

		for (int j = 0; j < 8; j++) {
			g->cars[i][j].pos.x = -1;
			g->cars[i][j].pos.y = -1;
		}
	}
		
	// 
}

DWORD WINAPI handleCommunication(LPVOID p) {
	ServerData* s = (ServerData*)p;

	// Generate game data
	
}