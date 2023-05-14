#include "Constants.h"

bool initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->hMemory = NULL;
	s->speed = -1;
	s->lanes = -1;
	s->gamemode = -1;
	s->g.exit = false;

	if (!(s->hMutex = CreateMutex(NULL, FALSE, SERVER_GAME_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	return true;
}

bool initMemoryDLL(HINSTANCE* h) {
	if (!(h = LoadLibrary(_T("Memory.dll"))))
		return false;

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