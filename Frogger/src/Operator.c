#include "Operator.h"

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

	// TODO Create shared memory for serverData memory
	if (!(s.hMemory = createSharedMemory(SERVER_MEMORY, sizeof(GameInfo)))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -3;
	}

	// TODO Operator shutdown handler
	FreeLibrary(hLib);
	CloseHandle(s.hMutexStop);
	CloseHandle(s.hMemory);
	CloseHandle(s.g.hMutex);

	return 0;
}