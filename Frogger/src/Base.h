#ifndef _BASE_H_
#define _BASE_H_

#include <Windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

void setUnicode() {
	#ifdef UNICODE
		(void)_setmode(_fileno(stdin), _O_WTEXT);
		(void)_setmode(_fileno(stdout), _O_WTEXT);
		(void)_setmode(_fileno(stderr), _O_WTEXT);
	#endif 
}

#endif