#include "Server.h"

int _tmain(int argc, TCHAR* argv[]) {
	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	ServerData serverData;
	HINSTANCE hLib = NULL;

	// Initialize memory DLL
	if (!initMemoryDLL(&hLib)) {
		_tprintf(_T("Error loading memory DLL!\n"));
		return -1;
	}

	// Initialize server data
	if (!initServerData(&serverData)) {
		_tprintf(_T("Error initializing serverData! Shutting down..."));
		return -2;
	}

	if (!(serverData.hMemory = createSharedMemory(SERVER_MEMORY, sizeof(GameInfo)))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -3;
	}

	// Check if only one server is running
	if (!isProgramUnique(&serverData.hMutex, SERVER_MUTEX)) {
		_tprintf(_T("Server already running! Shutting down..."));
		return -4;
	}

	// Get arguments from command arguments
	readArguments(&serverData, argc, argv);
	if (!handleRegistry(&serverData)) {
		_tprintf(_T("Error handling game registry keys"));
		return -5;
	}

	// TODO Create shared memory for serverData memory
	if (!(serverData.hMemory = createSharedMemory(SERVER_MEMORY, sizeof(GameInfo)))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -6;
	}

	// TODO Initialize thread for game handler
	if (!createThread(&(serverData.hThread), handleGame, &serverData)) {
		_tprintf(_T("Error creating game handler thread"));
		return -7;
	}

	// TODO Commands Handler
	handleCommands(&serverData);

	// TODO Proper shutdown server
	WaitForSingleObject(serverData.hThread, INFINITE);
	FreeLibrary(hLib);
	CloseHandle(serverData.hMutex);
	closeSharedMemory(serverData.hMemory, NULL);
	// TODO Inform clients that client is shutting down

	return 0;
}
