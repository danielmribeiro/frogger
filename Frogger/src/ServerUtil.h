#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include "Base.h"
#include "Constants.h"
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
	int num_lanes;
	int num_lanes_length;
	int num_frogs;
	int num_cars;
	int num_obstacles;
	Lane lanes[LANES_MAX];
	Frog frogs[FROGS_MAX];
	Car cars[CARS_MAX];
	Obstacle obstacle[OBSTACLES_MAX];
} FroggerGameboard;

*/

typedef struct {
	int x, y;
} Position;

typedef struct {
	Position pos;
} Car;

typedef struct {
	int level, lanes, speed, numCars[10];
	Car cars[10][8];
} GameInfo;

typedef struct {
	HANDLE hMutex, hThread, hMemory;
	GameInfo g;
	int clients, speed, lanes;
} ServerData;


void initServerData(ServerData* s);
bool initMemoryDLL(HINSTANCE* h);
bool isProgramUnique(HANDLE* hMutex, LPCSTR filename);
void readArguments(ServerData* s, int size, TCHAR* args[]);
bool handleRegistry(ServerData* s);
bool createThread(HANDLE* h, DWORD WINAPI f, LPVOID ptrData)


void generalFroggerThreadFunction(ServerData* s);

#endif