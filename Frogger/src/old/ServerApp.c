#include "ServerApp.h"

int _tmain(int argc, TCHAR *argv[]) {
	ServerAppData data;

	setUnicode();

	//WELCOME
	log(MSG_SERVERAPP_TITLE_FROGGER);
	
	// Arguments handler
	fill_default_serverappdata(&data);
	check_single_instance(&data);
	initialize_variables(argc, argv, &data);
	_tprintf(_T("\n\nSpeed - %d\nNumber Of Lanes - %d\n\n"), data.initialSpeed, data.initialNumberOfLanes);

	//INITIALIZE THREADS
	initialize_threads(&data);
	
	// Create shared memory
	SharedMemoryHandle sh = CreateSharedMemory(FROGGER_SERVER_MEMORY, sizeof(ServerMemoryData));

	// Create server thread for communication
	(void)_gettchar();

	//CLOSE SERVER
	close_serverapp(SUCCESS, &data);
}
