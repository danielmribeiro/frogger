#include "Server.h"

int _tmain(int argc, TCHAR* argv[]) {
	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	ServerData serverData;
	HINSTANCE hLib = NULL;
	HANDLE* serverMemory = NULL;

	// Initialize server data
	initServerData(&serverData);

	// Check if only one server is running
	if (!isProgramUnique(&serverData.hMutex, SERVER_MUTEX)) {
		_tprintf(_T("Server already running! Shutting down..."));
		return -1;
	}

	// Initialize memory DLL
	if (!initMemoryDLL(&hLib)) {
		_tprintf(_T("Error loading memory DLL!\n"));
		return -2;
	}

	// Get arguments from command arguments
	readArguments(&serverData, argc, argv);
	if (!handleRegistry(&serverData)) {
		_tprintf(_T("Error handling game registry keys"));
		return -3;
	}

	// TODO Create shared memory for serverData memory
	if (!(serverMemory = createSharedMemory(SERVER_MEMORY, sizeof(GameInfo)))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -4;
	}

	// TODO Initialize thread for game handler
	if (!createThread(&(serverData.hThread), handleGame, &serverData)) {
		_tprintf(_T("Error creating game handler thread"));
		return -5;
	}

	// TODO Commands Handler

	WaitForSingleObject(serverData.hThread, INFINITE);

	// TODO Close server app (Give warning for clients to shutdown, close handles and free all memory)
}
