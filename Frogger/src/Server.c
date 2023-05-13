#include "Server.h"

bool isServerUnique(HANDLE* hMutex) {
	hMutex = CreateMutex(NULL, TRUE, FROGGER_SERVERAPP_ONLINE);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return false;
	}

	return true;
}

typedef struct {
	HANDLE hMutex;
} ServerData;

int _tmain(int argc, TCHAR* argv[]) {
	ServerData serverData;

	setUnicode();

	// TODO Check if only one server is running
	if (!isServerUnique(&serverData.hMutex)) {
		return -1; // ERROR
	}

	// TODO Get arguments from command arguments
	// TODO When arguments are passed, create registry keys for those values
	// TODO If no arguments are passed, read from registry keys.
	// TODO If none shutdown

	// TODO Create shared memory for serverData memory

	// TODO  Initialize threads for communication and commands

	(void)_gettchar();

	// TODO Close server app (Give warning for clients to shutdown, close handles and free all memory)
}
