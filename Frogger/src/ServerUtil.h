#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include "Base.h"
#include "Constants.h"
#include "Memory.h"
#include "Registry.h"

bool initMemoryDLL(HINSTANCE* h);
bool isProgramUnique(HANDLE* hMutex, LPCSTR filename);
void readArguments(ServerData* s, int size, TCHAR* args[]);
bool handleRegistry(ServerData* s);
bool createThread(HANDLE* h, LPTHREAD_START_ROUTINE f, LPVOID ptrData);
void handleCommands(ServerData* data);

DWORD WINAPI handleGame(LPVOID p);
DWORD WINAPI handleComms(LPVOID p);
DWORD WINAPI handleClientsComms(LPVOID p);

#endif