#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include "Base.h"
#include "Constants.h"
#include "FroggerDLL.h"

typedef struct {
	int y;
	LaneType lane;
} Lane;

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

typedef struct {
	int initialNumberOfLanes;
	int initialSpeed;
	int currentNumberOfPlayersConnected;

	HANDLE hMutex;
	GameState gameState;
	FroggerGameboard fg;
} ServerMemoryData;

#endif