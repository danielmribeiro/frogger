#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include "Base.h"
#include "Constants.h"
#include "Registry.h"

typedef struct {
	HANDLE hMutex;
	int speed;
	int lanes;
} ServerData;

void initServerData(ServerData* s);
bool initMemoryDLL(HINSTANCE* h);
bool isProgramUnique(HANDLE* hMutex, LPCSTR filename);
void readArguments(ServerData* s, int size, TCHAR* args[]);
bool handleRegistry(ServerData* s);

#endif