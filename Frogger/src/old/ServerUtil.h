#ifndef _SERVERUTIL_H_
#define _SERVERUTIL_H_
#include "Base.h"
#include "Constants.h"

// DATA STRUCTURE

typedef struct {
	HANDLE hMutex;
	HANDLE hThreads[2];
	int isInitialNumberOfLanesFromArgs;
	int initialNumberOfLanesFromArgsValuePosition;
	int isInitialNumberOfLanesValueFromArgsValid;
	int initialNumberOfLanes;
	int isInitialSpeedFromArgs;
	int isInitialSpeedValueFromArgsValid;
	int initialSpeedFromArgsValuePosition;
	int initialSpeed;
	int currentNumberOfPlayersConnected;
	// FroggerGameboard froggerGameboard;
	int gameStarted;
	int gameIsRunning;
} ServerAppData;

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

void initialize_threads(ServerAppData* data);
DWORD WINAPI ThreadCommands(ServerAppData* data);
DWORD WINAPI ThreadConsume(ServerAppData* data);

void CreateDemoGame(ServerAppData* data);

#endif