#include "ServerApp.h"

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


	_gettchar();

	//CLOSE SERVER
	close_serverapp(SUCCESS, &data);
}
