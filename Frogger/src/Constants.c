#include "Constants.h"

bool initServerData(ServerData* s) {
	s->g.hMutex = NULL;
	s->hMutexStop = NULL;
	s->speed = -1;
	s->lanes = -1;
	s->gamemode = -1;
	s->hMemory = NULL;

	if (!(s->g.hMutex = CreateMutex(NULL, FALSE, SERVER_GAME_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	if (!(s->hMutexStop = CreateMutex(NULL, FALSE, SERVER_MEMORY_MUTEX))) {
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
