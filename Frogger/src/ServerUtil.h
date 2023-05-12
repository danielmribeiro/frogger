#ifndef _SERVERUTIL_H_
#define _SERVERUTIL_H_
#include "Base.h"
#include "Constants.h"

// DATA STRUCTURE

typedef struct {
	HANDLE hMutex;
	int isInitialNumberOfLanesFromArgs;
	int initialNumberOfLanesFromArgsValuePosition;
	int isInitialNumberOfLanesValueFromArgsValid;
	int initialNumberOfLanes;
	int isInitialSpeedFromArgs;
	int isInitialSpeedValueFromArgsValid;
	int initialSpeedFromArgsValuePosition;
	int initialSpeed;
	int currentNumberOfPlayersConnected;
} ServerAppData;

typedef enum {
	OFF,
	DEMO,
	INDIVIDUAL,
	COMPETITIVE
} GameState;

typedef enum {
	START,
	STREET,
	FINISH
} LaneType;

typedef struct {
	GameState gameState;
	int level;
	int score;
	int num_lanes;
	int num_lanes_length;
	int num_frogs;
	int num_cars;
	int num_obstacles;
	struct Lane {
		int y;
		LaneType lane;
	} lanes[LANES_MAX];
	struct Frog {
		int x;
		int y;
		int lives;
		TCHAR playerName;
	} frogs[FROGS_MAX];
	struct Car {
		int x;
		int y;
		int speed;
	}cars[CARS_MAX];
	struct Obstacle {
		int x;
		int y;
	} obstacle[OBSTACLES_MAX];
} FroggerGameboard;

typedef enum {
	RIGHT,
	LEFT,
	FRONT,
	BACK,
	STOP
} Direction;

typedef struct {
	TCHAR playerName;
	Direction lastMove;
} FroggerPlay;

void log(TCHAR* logString);
void fill_default_serverappdata(ServerAppData* data);
void check_single_instance(ServerAppData* data);
void initialize_variables(int argc, TCHAR* argv[], ServerAppData* data);
void check_valid_arguments_keys(int argc, TCHAR* argv[], ServerAppData* data);
void check_valid_arguments_values(TCHAR* argv[], ServerAppData* data);

void set_initial_speed(TCHAR* argv[], ServerAppData* data);
void set_initial_speed_from_args(TCHAR* argv[], ServerAppData* data);
void set_initial_speed_from_registry(ServerAppData* data);
void save_initial_speed_to_registry(ServerAppData* data);

void set_initial_number_of_lanes(TCHAR* argv[], ServerAppData* data);
void set_initial_number_of_lanes_from_args(TCHAR* argv[], ServerAppData* data);
void set_initial_number_of_lanes_from_registry(ServerAppData* data);
void save_initial_number_of_lanes_to_registry(ServerAppData* data);

int close_serverapp(int errorCode, ServerAppData* data);

#endif