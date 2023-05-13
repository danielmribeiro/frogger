#include "Server.h"

int _tmain(int argc, TCHAR* argv[]) {
	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);

	ServerData serverData;
	initServerData(&serverData);

	// TODO Check if only one server is running
	if (!isProgramUnique(&serverData.hMutex, SERVER_MUTEX)) {
		_tprintf(_T("Server already running! Shutting down..."));
		return -1;
	}

	// TODO Get arguments from command arguments
	readArguments(&serverData, argc, argv);
	if (!handleRegistry(&serverData))
		return -2;

	// TODO Create shared memory for serverData memory

	// TODO  Initialize threads for communication and commands

	(void)_gettchar();

	// TODO Close server app (Give warning for clients to shutdown, close handles and free all memory)
}
