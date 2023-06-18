#include "ClientComms.h"

DWORD WINAPI communicationHandlerProc(LPVOID* p) {
	return 0;
}

bool isServerRunning(ClientCommsData* c, GameInfo* g) {
	DWORD pid = CLIENT_CONNECT;

	// TODO fix wait named pipe connection forever!!! Can't be like this
	if (!WaitNamedPipe(SERVER_PIPE, 5000)) {
		_tprintf(_T("Error waiting to connect to %s pipe\n"), SERVER_PIPE);
		return false;
	}

	if ((c->hServerPipe = CreateFile(
		SERVER_PIPE,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
		) == INVALID_HANDLE_VALUE)
		return false;

	WriteFile(c->hServerPipe,
		&pid,
		sizeof(pid),
		NULL,
		NULL);

	return true;
}