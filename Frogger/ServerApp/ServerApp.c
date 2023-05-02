#include "ServerApp.h"

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
	
	data->isInitialNumberOfLanesFromArgs = FALSE;
	data->initialNumberOfLanesFromArgsValuePosition = -1;
	data->isInitialNumberOfLanesValueFromArgsValid = FALSE;

	data->isInitialSpeedFromArgs = FALSE;
	data->initialSpeedFromArgsValuePosition = -1;
	data->isInitialSpeedValueFromArgsValid = FALSE;
	
	data->initialSpeed = -1;
	data->initialNumberOfLanes = -1;

	data->currentNumberOfPlayersConnected = 0;
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

/// <summary>
/// Closes the ServerApp correctly
/// </summary>
/// <param name="errorCode"></param>
/// <param name="hMutex"></param>
/// <returns></returns>
int close_serverapp(int errorCode, ServerAppData* data) {
	log(MSG_SERVERAPP_TITLE_EXIT);
	switch (errorCode) {
	case SUCCESS:
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
	default:
		break;
	}
	log(MSG_SERVERAPP_INFO_CLOSE_SUCCESS);
	ReleaseMutex(data->hMutex);
	CloseHandle(data->hMutex);
	exit(errorCode);
}