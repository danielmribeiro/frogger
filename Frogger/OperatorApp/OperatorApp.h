#pragma once
#include <Windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//LOGIC
#define TRUE 1
#define FALSE 0

//TITLE MESSAGES
#define MSG_OPERATORAPP_TITLE_FROGGER _T("\n::::::: FROGGER - OperatorApp :::::::")
#define MSG_OPERATORAPP_TITLE_CHECK_SERVER_INSTANCE _T("\n------- CHECK SERVER INSTANCE -------")
#define MSG_OPERATORAPP_TITLE_EXIT _T("\n------- EXIT FROGGER - OPERATORAPP -------")

//INFO MESSAGES
#define MSG_OPERATORAPP_INFO_SERVERAPP_INSTANCE_RUNNING _T("[INFO]: There's a ServerApp instance running currently!")

//ERROR/CLOSE MESSAGES
#define MSG_OPERATORAPP_INFO_CLOSE_SUCCESS _T("[INFO]: OperatorApp is closing successfully!")
#define MSG_OPERATORAPP_ERROR_NO_SERVERAPP_INSTANCES_RUNNING _T("[ERROR]: There's no ServerApp instance running!")

//ERRORS
#define SUCCESS 0
#define ERROR_NO_SERVERAPP_INSTANCES_RUNNING 1

//MUTEX
#define FROGGER_SERVERAPP_ONLINE _T("FROGGER_SERVERAPP_ONLINE")