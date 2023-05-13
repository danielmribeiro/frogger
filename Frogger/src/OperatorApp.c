#include "OperatorApp.h"

void log(TCHAR* logString);
void check_serverapp_instance();
int close_operatorapp(int errorCode);

int _tmain(int argc, TCHAR* argv[]) {
	setUnicode();

	//WELCOME
	log(MSG_OPERATORAPP_TITLE_FROGGER);

	//CHECK SERVERAPP INSTANCE RUNNING
	check_serverapp_instance();

	_gettchar();

	//CLOSE OPERATOR
	close_operatorapp(SUCCESS);
}

/// <summary>
/// Prints log strings
/// </summary>
/// <param name="logString"></param>
void log(TCHAR* logString) {
	_tprintf(_T("%s\n"), logString);
}

/// <summary>
/// Checks if there's at least one ServerApp instance running currently
/// </summary>
/// <param name="data"></param>
void check_serverapp_instance() {
	log(MSG_OPERATORAPP_TITLE_CHECK_SERVER_INSTANCE);
	HANDLE hMutex = CreateMutex(NULL, TRUE, FROGGER_SERVERAPP_ONLINE);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		log(MSG_OPERATORAPP_INFO_SERVERAPP_INSTANCE_RUNNING);
	}
	else {
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
		close_operatorapp(ERROR_NO_SERVERAPP_INSTANCES_RUNNING);
	}
}

/// <summary>
/// Closes the OperatorApp correctly
/// </summary>
/// <param name="errorCode"></param>
/// <param name="data"></param>
/// <returns></returns>
int close_operatorapp(int errorCode) {
	log(MSG_OPERATORAPP_TITLE_EXIT);
	switch (errorCode) {
	case SUCCESS:
		log(MSG_OPERATORAPP_INFO_CLOSE_SUCCESS);
		break;
	case ERROR_NO_SERVERAPP_INSTANCES_RUNNING:
		log(MSG_OPERATORAPP_ERROR_NO_SERVERAPP_INSTANCES_RUNNING);
		break;
	default:
		break;
	}
	exit(errorCode);
}