#include "ClientComms.h"

DWORD WINAPI communicationHandlerProc(LPVOID* p) {
	return 0;
}

bool isServerRunning(ClientCommsData* c, GameInfo* g) {
	DWORD pid = 15;

	// TODO fix wait named pipe connection forever!!! Can't be like this
	if (!WaitNamedPipe(SERVER_PIPE, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(_T("Error waiting to connect to %s pipe\n"), SERVER_PIPE);
		return false;
	}

	// TODO open server pipe
	if ((c->hServerPipe = CreateFile(
		SERVER_PIPE,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
		) == INVALID_HANDLE_VALUE)
		return false;

	// TODO remove this. Debug only to check if pipe's working
	WriteFile(c->hServerPipe,
		&pid,
		sizeof(pid),
		NULL,
		NULL);
	pid = 10;
	WriteFile(c->hServerPipe,
		&pid,
		sizeof(pid),
		NULL,
		NULL);

	return true;
}