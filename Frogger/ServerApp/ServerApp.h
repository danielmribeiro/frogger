#pragma once
#include <Windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//MESSAGES
#define MSG_SERVERAPP_WELCOME _T("[INFO]: Welcome to the Frogger ServerApp!")
#define MSG_SERVERAPP_SINGLE_INSTANCE_RUNNING _T("[INFO]: This ServerApp instance is the only one running currently!")
#define MSG_SERVERAPP_MULTIPLE_INSTANCES_RUNNING _T("[INFO]: There's already a ServerApp instance running!")
#define MSG_SERVERAPP_CLOSE_SUCCESS _T("[INFO]: ServerApp is closing successfully!")

//ERRORS
#define SUCCESS 0