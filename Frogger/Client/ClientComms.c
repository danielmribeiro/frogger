#include "ClientComms.h"

DWORD WINAPI communicationHandlerProc(LPVOID* p) {
	return 0;
}

bool isServerRunning(ClientCommsData* c, GameInfo * g) {
	DWORD pid = 15;
	// TODO open server pipe
	if ((c->hServerPipe = CreateFile(
		SERVER_PIPE,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL)
		) == INVALID_HANDLE_VALUE)
		return false;

	// TODO create client pipe
	WriteFile(c->hServerPipe,
		&pid, 
		sizeof(pid), 
		NULL, 
		NULL);

	// TODO create communication handler thread

	return true;
}