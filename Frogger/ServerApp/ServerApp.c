#include "ServerApp.h"


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
	GameState gameState;
	int level;
	int score;
	int num_lanes;
	int num_lanes_length;
	int num_frogs;
	int num_cars;
	int num_obstacles;
	Lane lane[LANES_MAX];
	Frog frog[FROGS_MAX];
	Car cars[CARS_MAX];
	Obstacle obstacle[OBSTACLES_MAX];
} FroggerGameboard;

typedef enum {
	RIGHT,
	LEFT,
	FRONT,
	BACK,
	STOP
} Direction;

typedef struct {
	TCHAR playerName[128];
	Direction lastMove;
} FroggerPlay;

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
	FroggerGameboard froggerGameboard;
	int gameStarted;
	int gameIsRunning;
} ServerAppData;

void log(TCHAR *logString);
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

void initialize_threads(ServerAppData* data);
DWORD WINAPI ThreadCommands(ServerAppData* data);
DWORD WINAPI ThreadConsume(ServerAppData* data);

void CreateDemoGame(ServerAppData* data);

int close_serverapp(int errorCode, ServerAppData* data);

int _tmain(int argc, TCHAR *argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	//WELCOME
	log(MSG_SERVERAPP_TITLE_FROGGER);
	
	ServerAppData data;
	fill_default_serverappdata(&data);

	//CHECK SINGLE INSTANCE RUNNING
	check_single_instance(&data);

	//INITIALIZE VARIABLES
	initialize_variables(argc, argv, &data);

	//LIST SPEED AND NUMBER OF LANES
	_tprintf(_T("\n\nSpeed - %d\nNumber Of Lanes - %d\n\n"), data.initialSpeed, data.initialNumberOfLanes);

	//INITIALIZE THREADS
	initialize_threads(&data);

	_gettchar();

	//CLOSE SERVER
	close_serverapp(SUCCESS, &data);
}

/// <summary>
/// Prints log strings
/// </summary>
/// <param name="logString"></param>
void log(TCHAR *logString) {
	_tprintf(_T("%s\n"), logString);
}

/// <summary>
/// Fill the ServerAppData struct default values
/// </summary>
/// <param name="data"></param>
void fill_default_serverappdata(ServerAppData* data) {
	data->hMutex = NULL;

	data->hThreads[0] = NULL;
	data->hThreads[1] = NULL;
	
	data->isInitialNumberOfLanesFromArgs = FALSE;
	data->initialNumberOfLanesFromArgsValuePosition = -1;
	data->isInitialNumberOfLanesValueFromArgsValid = FALSE;

	data->isInitialSpeedFromArgs = FALSE;
	data->initialSpeedFromArgsValuePosition = -1;
	data->isInitialSpeedValueFromArgsValid = FALSE;
	
	data->initialSpeed = -1;
	data->initialNumberOfLanes = -1;

	data->currentNumberOfPlayersConnected = 0;

	data->gameStarted = FALSE;
	data->gameIsRunning = FALSE;

	
}

/// <summary>
/// Checks if there's only one ServerApp instance running currently
/// </summary>
/// <param name="hMutex"></param>
void check_single_instance(ServerAppData* data) {
	log(MSG_SERVERAPP_TITLE_CHECK_SINGLE_INSTANCE);
	data->hMutex = CreateMutex(NULL, TRUE, FROGGER_SERVERAPP_ONLINE);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(data->hMutex);
		close_serverapp(ERROR_MULTIPLE_INSTANCES_RUNNING, data);
	}
	else {
		log(MSG_SERVERAPP_INFO_SINGLE_INSTANCE_RUNNING);
	}
}

/// <summary>
/// Initialize the variables, by giving correct default values.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <param name="data"></param>
void initialize_variables(int argc, TCHAR* argv[], ServerAppData* data) {
	check_valid_arguments_keys(argc, argv, data);
	check_valid_arguments_values(argv, data);

	set_initial_speed(argv, data);
	set_initial_number_of_lanes(argv, data);
}

/// <summary>
/// Checks the number of arguments and if there's any values for the initial speed or the initial number of lanes.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <param name="data"></param>
void check_valid_arguments_keys(int argc, TCHAR* argv[], ServerAppData* data) {
	switch (argc) {
	case 1:
		data->isInitialNumberOfLanesFromArgs = FALSE;
		data->isInitialSpeedFromArgs = FALSE;
		break;
	case 3:
		if (_tcsicmp(argv[1], SPEED_STR) == 0) {
			data->isInitialNumberOfLanesFromArgs = FALSE;
			data->isInitialSpeedFromArgs = TRUE;
			data->initialSpeedFromArgsValuePosition = 2;
		}
		else if (_tcsicmp(argv[1], NUMBER_OF_LANES_STR) == 0) {
			data->isInitialNumberOfLanesFromArgs = TRUE;
			data->initialNumberOfLanesFromArgsValuePosition = 2;
			data->isInitialSpeedFromArgs = FALSE;
		}
		else {
			close_serverapp(ERROR_UNKNOWN_ARGUMENT, data);
		}
		break;
	case 5:
		if ((_tcsicmp(argv[1], SPEED_STR) == 0) && (_tcsicmp(argv[3], NUMBER_OF_LANES_STR) == 0)) {
			data->isInitialNumberOfLanesFromArgs = TRUE;
			data->initialNumberOfLanesFromArgsValuePosition = 4;
			data->isInitialSpeedFromArgs = TRUE;
			data->initialSpeedFromArgsValuePosition = 2;
		}
		else if ((_tcsicmp(argv[1], NUMBER_OF_LANES_STR) == 0) && (_tcsicmp(argv[3], SPEED_STR) == 0)) {
			data->isInitialNumberOfLanesFromArgs = TRUE;
			data->initialNumberOfLanesFromArgsValuePosition = 2;
			data->isInitialSpeedFromArgs = TRUE;
			data->initialSpeedFromArgsValuePosition = 4;
		}
		else {
			close_serverapp(ERROR_UNKNOWN_ARGUMENT, data);
		}
		break;
	default:
		close_serverapp(ERROR_INVALID_ARGUMENTS_QUANTITY, data);
		break;
	}
}

/// <summary>
/// Checks the values of the arguments for speed and number of lanes
/// </summary>
/// <param name="argv"></param>
/// <param name="data"></param>
void check_valid_arguments_values(TCHAR* argv[], ServerAppData* data) {
	int valuePosition = -1;

	//NUMBER OF LANES
	if (data->isInitialNumberOfLanesFromArgs == TRUE) {
		valuePosition = data->initialNumberOfLanesFromArgsValuePosition;
		if ((_ttoi(argv[valuePosition]) >= NUMBER_OF_LANES_MIN) && (_ttoi(argv[valuePosition]) <= NUMBER_OF_LANES_MAX)) {
			data->isInitialNumberOfLanesValueFromArgsValid = TRUE;
		}
		else {
			close_serverapp(ERROR_INVALID_NUMBER_OF_LANES,data);
		}
	}

	//SPEED
	if (data->isInitialSpeedFromArgs == TRUE) {
		valuePosition = data->initialSpeedFromArgsValuePosition;
		if ((_ttoi(argv[valuePosition]) >= SPEED_MIN) && (_ttoi(argv[valuePosition]) <= SPEED_MAX)) {
			data->isInitialSpeedValueFromArgsValid = TRUE;
		}
		else {
			close_serverapp(ERROR_INVALID_SPEED, data);
		}
	}
}

/// <summary>
/// Set the Initial Speed
/// </summary>
/// <param name="argv"></param>
/// <param name="data"></param>
void set_initial_speed(TCHAR* argv[], ServerAppData* data) {
	log(MSG_SERVERAPP_TITLE_SPEED);

	//SET INITIAL SPEED
	if (data->isInitialSpeedFromArgs == TRUE && data->isInitialSpeedValueFromArgsValid == TRUE) {
		log(MSG_SERVERAPP_TITLE_SET_SPEED_FROM_ARGUMENTS);
		set_initial_speed_from_args(argv, data);
	}
	else {
		log(MSG_SERVERAPP_TITLE_SET_SPEED_FROM_REGISTRY);
		set_initial_speed_from_registry(data);
	}

	//SAVE INITIAL SPEED
	log(MSG_SERVERAPP_TITLE_SAVE_SPEED);
	save_initial_speed_to_registry(data);
}

/// <summary>
/// Set the value of initial speed from the arguments
/// </summary>
/// <param name="argv"></param>
/// <param name="data"></param>
void set_initial_speed_from_args(TCHAR* argv[], ServerAppData* data) {
	data->initialSpeed = _ttoi(argv[data->initialSpeedFromArgsValuePosition]);
	log(MSG_SERVERAPP_INFO_SPEED_SET);
}

/// <summary>
/// Set the initial speed value from the registry
/// </summary>
/// <param name="data"></param>
void set_initial_speed_from_registry(ServerAppData* data) {
	TCHAR keyPath[sizeof(KEYPATH_NAME)] = KEYPATH_NAME;
	TCHAR speed_value[sizeof(SPEED_KEY_NAME)] = SPEED_KEY_NAME;
	HKEY hkey;
	DWORD defaultSize;
	DWORD res;

	DWORD speed_data;

	//GET VALUE FROM REGISTRY
	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_SPEED_KEY_OPENED);
		if (RegQueryValueEx(hkey, speed_value, NULL, NULL, (LPBYTE)&speed_data, &defaultSize) == ERROR_SUCCESS) {
			log(MSG_SERVERAPP_INFO_SPEED_KEY_READ);
		}
		else {
			RegCloseKey(hkey);
			log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
			close_serverapp(ERROR_INVALID_KEY_SPEED, data);
		}
	}
	else {
		RegCloseKey(hkey);
		log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
		close_serverapp(ERROR_CANT_OPEN_KEY,data);
	}

	//GIVE VALUE TO VARIABLE IN SERVERDATA STRUCT
	data->initialSpeed = speed_data;
	log(MSG_SERVERAPP_INFO_SPEED_SET);

	RegCloseKey(hkey);
	log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
	
}

/// <summary>
/// Save the value of the initial speed in the registry
/// </summary>
/// <param name="data"></param>
void save_initial_speed_to_registry(ServerAppData* data) {
	TCHAR keyPath[sizeof(KEYPATH_NAME)] = KEYPATH_NAME;
	TCHAR speed_value[sizeof(SPEED_KEY_NAME)] = SPEED_KEY_NAME;
	HKEY hkey;
	DWORD defaultSize;
	DWORD res;

	DWORD speed_data = data->initialSpeed;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_SPEED_KEY_OPENED);
	}
	else {
		log(MSG_SERVERAPP_ERROR_CANT_OPEN_KEY);
		if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &res) == ERROR_SUCCESS) {
			log(MSG_SERVERAPP_INFO_SPEED_KEY_CREATED);
		}
		else {
			RegCloseKey(hkey);
			log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
			close_serverapp(ERROR_CANT_CREATE_KEY, data);
		}
	}

	if (RegSetValueEx(hkey, speed_value, 0, REG_DWORD, (LPBYTE)&speed_data, sizeof(speed_data)) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_SPEED_KEY_SET);
	}
	else {
		RegCloseKey(hkey);
		log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
		close_serverapp(ERROR_CANT_SET_KEY, data);
	}

	RegCloseKey(hkey);
	log(MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED);
}

/// <summary>
/// Set the initial number of lanes
/// </summary>
/// <param name="argv"></param>
/// <param name="data"></param>
void set_initial_number_of_lanes(TCHAR* argv[], ServerAppData* data) {
	log(MSG_SERVERAPP_TITLE_NUMBER_OF_LANES);

	//SET INITIAL NUMBER OF LANES
	if (data->isInitialNumberOfLanesFromArgs == TRUE && data->isInitialNumberOfLanesValueFromArgsValid == TRUE) {
		log(MSG_SERVERAPP_TITLE_SET_NUMBER_OF_LANES_FROM_ARGUMENTS);
		set_initial_number_of_lanes_from_args(argv, data);
	}
	else {
		log(MSG_SERVERAPP_TITLE_SET_NUMBER_OF_LANES_FROM_REGISTRY);
		set_initial_number_of_lanes_from_registry(data);
	}

	//SAVE INITIAL NUMBER OF LANES
	log(MSG_SERVERAPP_TITLE_SAVE_NUMBER_OF_LANES);
	save_initial_number_of_lanes_to_registry(data);
}

/// <summary>
/// Set the value of the initial number of lanes from the arguments
/// </summary>
/// <param name="argv"></param>
/// <param name="data"></param>
void set_initial_number_of_lanes_from_args(TCHAR* argv[], ServerAppData* data) {
	data->initialNumberOfLanes = _ttoi(argv[data->initialNumberOfLanesFromArgsValuePosition]);
	log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_SET);
}

/// <summary>
/// Set the initial number of lanes value from the registry
/// </summary>
/// <param name="data"></param>
void set_initial_number_of_lanes_from_registry(ServerAppData* data) {
	TCHAR keyPath[sizeof(KEYPATH_NAME)] = KEYPATH_NAME;
	TCHAR numberOfLanes_value[sizeof(NUMBER_OF_LANES_KEY_NAME)] = NUMBER_OF_LANES_KEY_NAME;
	HKEY hkey;
	DWORD defaultSize;
	DWORD res;

	DWORD numberOfLanes_data;

	//GET VALUE FROM REGISTRY
	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_OPENED);
		if (RegQueryValueEx(hkey, numberOfLanes_value, NULL, NULL, (LPBYTE)&numberOfLanes_data, &defaultSize) == ERROR_SUCCESS) {
			log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_READ);
		}
		else {
			RegCloseKey(hkey);
			log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
			close_serverapp(ERROR_INVALID_KEY_NUMBER_OF_LANES, data);
		}
	}
	else {
		RegCloseKey(hkey);
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
		close_serverapp(ERROR_CANT_OPEN_KEY, data);
	}

	//GIVE VALUE TO VARIABLE IN SERVERDATA STRUCT
	data->initialNumberOfLanes = numberOfLanes_data;
	log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_SET);

	RegCloseKey(hkey);
	log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
}

/// <summary>
/// Save the value of the initial number of lanes in the registry
/// </summary>
/// <param name="data"></param>
void save_initial_number_of_lanes_to_registry(ServerAppData* data){
	TCHAR keyPath[sizeof(KEYPATH_NAME)] = KEYPATH_NAME;
	TCHAR numberOfLanes_value[sizeof(NUMBER_OF_LANES_KEY_NAME)] = NUMBER_OF_LANES_KEY_NAME;
	HKEY hkey;
	DWORD defaultSize;
	DWORD res;

	DWORD numberOfLanes_data = data->initialNumberOfLanes;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_OPENED);
	}else{
		log(MSG_SERVERAPP_ERROR_CANT_OPEN_KEY);
		if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &res) == ERROR_SUCCESS) {
			log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CREATED);
		}
		else {
			RegCloseKey(hkey);
			log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
			close_serverapp(ERROR_CANT_CREATE_KEY, data);
		}
	}

	if (RegSetValueEx(hkey, numberOfLanes_value, 0, REG_DWORD, (LPBYTE)&numberOfLanes_data, sizeof(numberOfLanes_data)) == ERROR_SUCCESS) {
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_SET);
	}
	else {
		RegCloseKey(hkey);
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
		close_serverapp(ERROR_CANT_SET_KEY, data);
	}

	RegCloseKey(hkey);
	log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED);
}

void initialize_threads(ServerAppData* data) {
	data->hThreads[0] = CreateThread(NULL, 0, ThreadCommands, data, 0, NULL);
	data->hThreads[1] = CreateThread(NULL, 0, ThreadConsume, data, 0, NULL);
	WaitForMultipleObjects(2, data->hThreads, TRUE, INFINITE);
}

/// <summary>
/// Thread responsable to Write the Server Commands
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
DWORD WINAPI ThreadCommands(ServerAppData* data) {
	TCHAR serverCmd[SIZEOF_SERVERCMD];
	while (1) {
		log(MSG_SERVERAPP_INTERACTION_INSERT_COMMAND);
		_fgetts(serverCmd, SIZEOF_SERVERCMD, stdin);
		_tprintf(TEXT("Handling Command: %s\n"), serverCmd);
		if (_tcsicmp(COMMAND_DEMO, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_DEMO);
			StartGame(data, GAME_DEMO);
		}
		else if (_tcsicmp(COMMAND_SUSPEND, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_SUSPEND);
			SuspendGame(data);
		}
		else if (_tcsicmp(COMMAND_RESUME, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_RESUME);
			ResumeGame(data);
		}
		else if (_tcsicmp(COMMAND_RESTART, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_RESTART);
			RestartGame(data);
		}
		else if (_tcsicmp(COMMAND_EXIT, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_EXIT);
			ExitGame(data, TRUE);
		}
		else if (_tcsicmp(COMMAND_QUIT, serverCmd) == 0) {
			log(MSG_SERVERAPP_INFO_COMMAND_QUIT);
			close_serverapp(ERROR_SUCCESS_COMMAND_QUIT, data);
		}
		else {
			log(MSG_SERVERAPP_INFO_COMMAND_NOT_FOUND);
		}
	}
}


void SuspendGame(ServerAppData* data) {
	if (data->gameStarted == FALSE) {
		log(MSG_SERVERAPP_INFO_SUSPEND_GAME_NOT_STARTED);
	}
	else {
		if (data->gameIsRunning == FALSE) {
			log(MSG_SERVERAPP_INFO_SUSPEND_GAME_NOT_RUNNING);
		}
		else {
			data->gameIsRunning = FALSE;
		}
	}
}

void ResumeGame(ServerAppData* data) {
	if (data->gameStarted == FALSE) {
		log(MSG_SERVERAPP_INFO_RESUME_GAME_NOT_STARTED);
	}
	else {
		if (data->gameIsRunning == TRUE) {
			log(MSG_SERVERAPP_INFO_RESUME_GAME_RUNNING);
		}
		else {
			data->gameIsRunning = TRUE;
		}
	}
}

void RestartGame(ServerAppData* data) {
	if (data->gameStarted == FALSE) {
		log(MSG_SERVERAPP_INFO_RESTART_GAME_NOT_STARTED);
	}
	else {
		ExitGame(data, TRUE);
		int connectedPlayers = data->currentNumberOfPlayersConnected;
		switch (connectedPlayers) {
		case 0:
			StartGame(data, GAME_DEMO);
			break;
		case 1:
			StartGame(data, GAME_INDIVIDUAL);
			break;
		case 2:
			StartGame(data, GAME_COMPETITIVE);
			break;
		default:
			break;
		}
	}
}

void ExitGame(ServerAppData* data, int saveConnectedPlayers) {
	if (data->gameStarted == FALSE) {
		log(MSG_SERVERAPP_INFO_EXIT_GAME_NOT_STARTED);
	}
	else {
		if (saveConnectedPlayers == FALSE) {
			DisconnectAllPlayers(data);
		}
		data->gameIsRunning = FALSE;
		data->gameStarted = FALSE;
	}
}

void DisconnectAllPlayers(ServerAppData* data) {
	log(MSG_SERVERAPP_INFO_NOT_IMPLEMENTED);
}

void DisconnectAllOperators(ServerAppData* data) {
	//TODO: LOGIC HERE
}

void StartGame(ServerAppData* data, TCHAR* typeOfGame) {
	if (data->gameStarted == TRUE || data->gameIsRunning == TRUE) {
		log(MSG_SERVERAPP_INFO_START_GAME_STARTED_RUNNING);
	}
	else {
		if (_tcsicmp(typeOfGame, GAME_DEMO) == 0) {
			StartDemo(data);
		}
		else if (_tcsicmp(typeOfGame, GAME_INDIVIDUAL) == 0) {
			log(MSG_SERVERAPP_INFO_NOT_IMPLEMENTED);
		}
		else if (_tcsicmp(typeOfGame, GAME_COMPETITIVE) == 0) {
			log(MSG_SERVERAPP_INFO_NOT_IMPLEMENTED);
		}
	}
}


void StartDemo(ServerAppData* data) {
	data->froggerGameboard.gameState = DEMO;
	data->froggerGameboard.level = 1;
	data->froggerGameboard.score = 0;
	data->froggerGameboard.num_lanes = STARTLINE_DEFAULT + data->initialNumberOfLanes + FINISHLINE_DEFAULT;
	data->froggerGameboard.num_lanes_length = LANES_LENGTH;
	data->froggerGameboard.num_frogs = 2;
	data->froggerGameboard.num_cars = 3;
	data->froggerGameboard.num_obstacles = 2;

	//START LANES
	for (int i = 0; i < STARTLINE_DEFAULT; i++) {
		data->froggerGameboard.lane[i].lane = START;
		data->froggerGameboard.lane[i].y = i;
	}
	//STREET LANES
	for (int i = STARTLINE_DEFAULT; i < STARTLINE_DEFAULT + data->initialNumberOfLanes; i++) {
		data->froggerGameboard.lane[i].lane = STREET;
		data->froggerGameboard.lane[i].y = i;
	}
	//FINISH LANES
	for (int i = STARTLINE_DEFAULT + data->initialNumberOfLanes; i < data->froggerGameboard.num_lanes; i++) {
		data->froggerGameboard.lane[i].lane = FINISH;
		data->froggerGameboard.lane[i].y = i;
	}

	//FIRST FROG
	data->froggerGameboard.frog[0].lives = 1;
	_tcscpy_s(data->froggerGameboard.frog[0].playerName, _countof(data->froggerGameboard.frog[0].playerName), PLAYER1);
	data->froggerGameboard.frog[0].x = 0;
	data->froggerGameboard.frog[0].y = 0;

	//SECOND FROG
	data->froggerGameboard.frog[1].lives = 1;
	_tcscpy_s(data->froggerGameboard.frog[1].playerName, _countof(data->froggerGameboard.frog[1].playerName), PLAYER2);
	data->froggerGameboard.frog[1].x = 1;
	data->froggerGameboard.frog[1].y = 0;

	//CARS
	int index = 0;
	for (int i = STARTLINE_DEFAULT; i < STARTLINE_DEFAULT + data->initialNumberOfLanes; i++) {
		for (int j = 0; j < data->froggerGameboard.num_cars; j++) {
			index = ((i - STARTLINE_DEFAULT) * data->froggerGameboard.num_cars) + j;
			data->froggerGameboard.cars[index].speed = data->initialSpeed;
			data->froggerGameboard.cars[index].x = i;
			data->froggerGameboard.cars[index].y = j;
		}
	}

	//OBSTACLES
	for (int i = STARTLINE_DEFAULT; i < STARTLINE_DEFAULT + data->initialNumberOfLanes; i++) {
		for (int j = data->froggerGameboard.num_cars; j < data->froggerGameboard.num_cars + data->froggerGameboard.num_obstacles; j++) {
			index = ((i - STARTLINE_DEFAULT) * data->froggerGameboard.num_obstacles) + j - data->froggerGameboard.num_cars;
			data->froggerGameboard.obstacle[index].x = i;
			data->froggerGameboard.obstacle[index].y = j;
		}
	}
	//TODO LOGIC HERE
}


/// <summary>
/// Thread responsable to Consume the Operator and Client Apps
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
DWORD WINAPI ThreadConsume(ServerAppData* data) {}

/// <summary>
/// Closes the ServerApp correctly
/// </summary>
/// <param name="errorCode"></param>
/// <param name="hMutex"></param>
/// <returns></returns>
int close_serverapp(int errorCode, ServerAppData* data) {
	log(MSG_SERVERAPP_TITLE_EXIT);
	DisconnectAllPlayers(data);
	DisconnectAllOperators(data);
	switch (errorCode) {
	case SUCCESS:
		log(MSG_SERVERAPP_INFO_CLOSE_SUCCESS);
		break;
	case ERROR_MULTIPLE_INSTANCES_RUNNING:
		log(MSG_SERVERAPP_ERROR_MULTIPLE_INSTANCES_RUNNING);
		break;
	case ERROR_UNKNOWN_ARGUMENT:
		log(MSG_SERVERAPP_ERROR_UNKNOWN_ARGUMENT);
		log(MSG_SERVERAPP_INFO_ARGUMENT_FORMAT);
		break;
	case ERROR_INVALID_ARGUMENTS_QUANTITY:
		log(MSG_SERVERAPP_ERROR_INVALID_ARGUMENTS_QUANTITY);
		log(MSG_SERVERAPP_INFO_ARGUMENT_FORMAT);
		break;
	case ERROR_INVALID_NUMBER_OF_LANES:
		log(MSG_SERVERAPP_ERROR_INVALID_NUMBER_OF_LANES);
		log(MSG_SERVERAPP_INFO_NUMBER_OF_LANES_ARGUMENT_LIMIT);
		break;
	case ERROR_INVALID_SPEED:
		log(MSG_SERVERAPP_ERROR_INVALID_SPEED);
		log(MSG_SERVERAPP_INFO_SPEED_ARGUMENT_LIMIT);
		break;
	case ERROR_INVALID_KEY_SPEED:
		log(MSG_SERVERAPP_ERROR_INVALID_KEY_SPEED);
		break;
	case ERROR_INVALID_KEY_NUMBER_OF_LANES:
		log(MSG_SERVERAPP_ERROR_INVALID_KEY_NUMBER_OF_LANES);
		break;
	case ERROR_CANT_OPEN_KEY:
		log(MSG_SERVERAPP_ERROR_CANT_OPEN_KEY);
		break;
	case ERROR_CANT_CREATE_KEY:
		log(MSG_SERVERAPP_ERROR_CANT_CREATE_KEY);
		break;
	case ERROR_CANT_SET_KEY:
		log(MSG_SERVERAPP_ERROR_CANT_SET_KEY);
		break;
	case ERROR_SUCCESS_COMMAND_QUIT:
		//TODO CLOSE ALL THE STUFF
		log(MSG_SERVERAPP_INFO_CLOSE_SUCCESS);
		break;
	default:
		break;
	}
	ReleaseMutex(data->hMutex);
	CloseHandle(data->hMutex);
	exit(errorCode);
}