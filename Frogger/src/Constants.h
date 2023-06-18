#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "Base.h"

#define MAX_PLAYERS 2

#define SERVER_MUTEX _T("SERVER_MUTEX")
#define SERVER_MEMORY _T("SERVER_MEMORY")
#define SERVER_GAME_MUTEX _T("SERVER_GAME_MUTEX")
#define SERVER_MEMORY_MUTEX _T("SERVER_MEMORY_MUTEX")
#define SERVER_MEMORY_BUFFER _T("SERVER_MEMORY_BUFFER")
#define EVENT_SERVER_SHUTDOWN _T("SERVER_SHUTDOWN")
#define EVENT_GAME_IS_UPDATED _T("GAME_IS_UPDATED")

#define SERVER_PIPE _T("\\\\.\\pipe\\SERVER_PIPE")
#define CLIENT_PIPE _T("\\\\.\\pipe\\CLIENT_PIPE_%d")

#define READ_SEMAPHORE _T("READ_SEMAPHORE")
#define WRITE_SEMAPHORE _T("WRITE_SEMAPHORE")

#define OPERATOR_COMMAND_MUTEX _T("OPERATOR_COMMAND_MUTEX")
#define EVENT_COMMAND_UPDATED _T("EVENT_COMMAND_UPDATED_%lu")

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
#define MAX_OBSTACLES 20
#define BUF_SIZE 10

typedef enum {
	INSERT_OBSTACLE,
	STOP_CARS,
	INVERT_CARS
} BufferMsgType;

typedef struct {
	int type;
	TCHAR message[255];
} CircularBuffer;

typedef struct {
	int indexWrite, indexRead;
	CircularBuffer circBuf[BUF_SIZE];
} CircularBufferMemory;

typedef struct {
	Position pos;
} Obstacle;

typedef struct {
	Position pos;
	TCHAR username[50];
	int score;
} Frog;

typedef struct {
	bool exit, isCarsRunning;
	int time, level, lanes, speed, numObs[MAX_LANES], numCars[MAX_LANES];
	Car cars[MAX_LANES][MAX_CARS];
	Frog frogs[MAX_PLAYERS];
	Obstacle obstacles[MAX_LANES][MAX_OBSTACLES];
} GameInfo;

typedef struct ClientPipe ClientPipe;
typedef struct {
	HANDLE hGameThread, hCommsThread, hMemory, hMutex, hCircBuf;
	HANDLE hClientsComms;
	HANDLE hEventGameIsUpdated, hEventServerShutdown;
	ClientPipe* clientPipes;
	GameInfo g;
	int clients, speed, lanes, status, gamemode;
	// STATUS: WAITING = 0, GAME_IS_RUNNING = 1, EXIT = 2
} ServerData;

struct ClientPipe {
	DWORD playerID;
	BOOL isActive;
	ServerData* s;
	HANDLE hPipe;
};

enum ClientRequestType {
	CLIENT_SHUTDOWN,
	CLIENT_CONNECT,
	PLAY_INDIVIDUAL,
	PLAY_COMPETITIVE,
	GAME_UPDATE,
	GAME_WIN,
	GAME_LOSE
};

typedef struct {
	DWORD type;
	// TODO add gameInfo
} ClientRequest;

// Operator Command Reader Structure
typedef struct {
	int* status;
	HANDLE* hEventHasCommand, * hCommandMutex;
	TCHAR* command;
} CommandReaderData;


//REGISTRY
#define KEYPATH _T("SOFTWARE\\Frogger\\")
#define LANES_KEY_NAME _T("NUMBEROFLANES")
#define SPEED_KEY_NAME _T("SPEED")

// SERVER DATA
#define SPEED_MIN 5
#define SPEED_MAX 120
#define SPEED_STOPPED 0

//LIVES
#define LIVES_MAX 3
#define LIVES_DEFAULT LIVES_DEFAULT

//GAMES
#define WAITING_FOR_GAME 0
#define GAME_RUNNING 1
#define GAME_FINISHED 2
#define SHUTDOWN 3

//COMMANDS
#define COMMAND_QUIT _T("QUIT\n")
#define COMMAND_DEMO _T("DEMO\n")
#define COMMAND_START _T("START\n")
#define COMMAND_SUSPEND _T("SUSPEND\n")
#define COMMAND_RESUME _T("RESUME\n")
#define COMMAND_RESTART _T("RESTART\n")
#define COMMAND_EXIT _T("EXIT\n")
#define COMMAND_STOP _T("STOP\n")
#define COMMAND_INVERT _T("INVERT\n")
#define COMMAND_INSERT _T("INSERT\n")

bool initServerData(ServerData* s, bool isServer);
bool initMemoryDLL(HINSTANCE* h);
bool createThread(HANDLE* h, LPTHREAD_START_ROUTINE f, LPVOID ptrData);

#endif