#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include "Base.h"
#include "Constants.h"
#include "Memory.h"
#include "Registry.h"

/*
typedef struct {
	int x;
	int y;
	int lives;
	TCHAR playerName[128];
} Frog;

typedef struct {
	int x;
	int y;
	int speed;
} Car;

typedef struct {
	int x;
	int y;
} Obstacle;

typedef struct {
	int level;
	int score;
	int num_frogs;
	int num_cars;
	int num_obstacles;
	Frog frogs[FROGS_MAX];
	Car cars[CARS_MAX];
	Obstacle obstacle[OBSTACLES_MAX];
} FroggerGameboard;

*/

bool initServerData(ServerData* s);
bool initMemoryDLL(HINSTANCE* h);
bool isProgramUnique(HANDLE* hMutex, LPCSTR filename);
void readArguments(ServerData* s, int size, TCHAR* args[]);
bool handleRegistry(ServerData* s);
bool createThread(HANDLE* h, LPTHREAD_START_ROUTINE f, LPVOID ptrData);
void handleCommands(ServerData* data);

DWORD WINAPI handleGame(LPVOID p);

#endif