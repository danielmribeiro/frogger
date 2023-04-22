#include "ServerApp.h"

void log(TCHAR* logString);
int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	log(MSG_SERVERAPP_WELCOME);
	log(MSG_SERVERAPP_CLOSE_SUCCESS);

	exit(SUCCESS);
}
/// <summary>
/// Prints log strings
/// </summary>
/// <param name="logString"></param>
void log(TCHAR* logString) {
	_tprintf(_T("%s\n"), logString);
}