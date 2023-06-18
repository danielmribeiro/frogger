#include "ClientComms.h"

bool isServerRunning(ClientCommsData* c) {
	DWORD pid = CLIENT_CONNECT;

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