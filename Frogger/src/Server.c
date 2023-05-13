#include "Server.h"

int _tmain(int argc, TCHAR* argv[]) {
	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	ServerData serverData;
	ServerMemoryData serverMemoryData;
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
	TCHAR * pathDLL = _T("Memory.dll");
	hLib = LoadLibrary(pathDLL);

	if (hLib == NULL) {
		_ftprintf_s(stderr, TEXT("Erro a carregar a DLL!\n"));
		return 0;
	}

	// Get arguments from command arguments
	readArguments(&serverData, argc, argv);
	if (!handleRegistry(&serverData))
		return -2;

	// TODO Create shared memory for serverData memory
	if (!(serverMemory = createSharedMemory(SERVER_MEMORY, sizeof(ServerMemoryData)))) {
		_tprintf(_T("Error creating shared memory file! Shutting down..."));
		return -3;
	}

	// TODO  Initialize threads for communication and commands

	(void)_gettchar();

	// TODO Close server app (Give warning for clients to shutdown, close handles and free all memory)
}
