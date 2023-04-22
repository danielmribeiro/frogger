#include "ServerApp.h"

void log(TCHAR* logString);
void check_single_instance(HANDLE hMutex);
int close_serverapp(int errorCode);

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	//WELCOME
	log(MSG_SERVERAPP_WELCOME);
	
	//CHECK SINGLE INSTANCE RUNNING
	HANDLE hMutex = NULL;
	check_single_instance(hMutex);

	//CLOSE SERVER
	close_serverapp(SUCCESS, hMutex);
}

/// <summary>
/// Prints log strings
/// </summary>
/// <param name="logString"></param>
void log(TCHAR* logString) {
	_tprintf(_T("%s\n"), logString);
}

/// <summary>
/// Checks if there's only one ServerApp instance running currently
/// </summary>
/// <param name="hMutex"></param>
void check_single_instance(HANDLE hMutex) {
	hMutex = CreateMutex(NULL, TRUE, FROGGER_SERVERAPP_ONLINE);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		log(MSG_SERVERAPP_MULTIPLE_INSTANCES_RUNNING);
		close_serverapp(ERROR_MULTIPLE_INSTANCES_RUNNING, hMutex);
	}
	else {
		log(MSG_SERVERAPP_SINGLE_INSTANCE_RUNNING);
	}
}

/// <summary>
/// Closes the ServerApp correctly
/// </summary>
/// <param name="errorCode"></param>
/// <param name="hMutex"></param>
/// <returns></returns>
int close_serverapp(int errorCode, HANDLE hMutex) {
	switch (errorCode) {
	case SUCCESS:
		break;
	case ERROR_MULTIPLE_INSTANCES_RUNNING:
		break;
	default:
		break;
	}
	log(MSG_SERVERAPP_CLOSE_SUCCESS);
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	exit(errorCode);
}