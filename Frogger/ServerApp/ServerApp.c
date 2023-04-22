#include "ServerApp.h"

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif 

	_tprintf(_T("%s\n"), MSG_SERVERAPP_WELCOME);
	_tprintf(_T("%s\n"), MSG_SERVERAPP_CLOSE_SUCCESS);

	exit(SUCCESS);
}