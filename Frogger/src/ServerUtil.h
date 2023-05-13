#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include "Base.h"

typedef struct {
	HANDLE hMutex;

	int speed;
	int lanes;
} ServerData;

void initServerData(ServerData* s);
bool isProgramUnique(HANDLE* hMutex, LPCSTR filename);
void readArguments(ServerData* s, int size, TCHAR* args[]);

#endif