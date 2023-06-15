#ifndef _CLIENT_COMMS_H
#define _CLIENT_COMMS_H

#include "../src/Base.h"
#include "../src/Constants.h"

typedef struct {
	HANDLE hServerPipe, hClientPipe, hCommunicationThread;
} ClientCommsData;

bool isServerRunning(ClientCommsData* c, GameInfo * g);

#endif