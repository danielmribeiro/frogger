#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Base.h"

#define SERVER_MUTEX _T("SERVER_MUTEX")
#define SERVER_MEMORY _T("SERVER_MEMORY")
#define SERVER_GAME_MUTEX _T("SERVER_GAME_MUTEX")
#define SERVER_MEMORY_MUTEX _T("SERVER_MEMORY_MUTEX")

typedef enum {
	OFF,
	DEMO,
	INDIVIDUAL,
	COMPETITIVE
} GameState;

typedef struct {
	int x, y;
} Position;

typedef enum {
	RIGHT,
	LEFT,
	FRONT,
	BACK
} Direction;

typedef struct {
	Position pos;
	Direction dir;
} Car;

#define MAX_LANES 10
#define MAX_CARS 8

typedef struct {
	HANDLE hMutex; // CONTROL MUTEX 
	int level, lanes, speed, numCars[MAX_LANES];
	Car cars[MAX_LANES][MAX_CARS];
} GameInfo;

typedef struct {
	HANDLE hThread, hMemory, hMutexStop;
	GameInfo g;
	int clients, speed, lanes, status, gamemode;
	// STATUS: RUN = 0, EXIT = 1
} ServerData;

//REGISTRY
#define KEYPATH _T("SOFTWARE\\Frogger\\")
#define SPEED_KEY_NAME _T("SPEED")
#define LANES_KEY_NAME _T("NUMBEROFLANES")

// SERVER DATA
#define SPEED_MIN 5
#define SPEED_MAX 120
#define SPEED_STOPPED 0

//LIVES
#define LIVES_MAX 3
#define LIVES_DEFAULT LIVES_DEFAULT

//GAMES
#define GAME_DEMO 0
#define GAME_INDIVIDUAL 1
#define GAME_COMPETITIVE 2

//COMMANDS
#define COMMAND_QUIT _T("QUIT\n")
#define COMMAND_DEMO _T("DEMO\n")
#define COMMAND_SUSPEND _T("SUSPEND\n")
#define COMMAND_RESUME _T("RESUME\n")
#define COMMAND_RESTART _T("RESTART\n")
#define COMMAND_EXIT _T("EXIT\n")

bool initServerData(ServerData* s);
bool initMemoryDLL(HINSTANCE* h);

#endif