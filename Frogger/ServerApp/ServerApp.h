#pragma once
#include <Windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//INFO MESSAGES
#define MSG_SERVERAPP_FROGGER _T("::::::: FROGGER - ServerApp :::::::")
#define MSG_SERVERAPP_SINGLE_INSTANCE_RUNNING _T("[INFO]: This ServerApp instance is the only one running currently!")
#define MSG_SERVERAPP_CLOSE_SUCCESS _T("[INFO]: ServerApp is closing successfully!")
#define MSG_SERVERAPP_ARGUMENT_FORMAT _T("[INFO]: You should use one of the following options:\n\t./ServerApp\n\t\tExample: ./ServerApp\n\n\t./ServerApp -s [SPEED]\n\t\tExample: ./ServerApp 50\n\n\t./ServerApp -l [NUMBER_OF_LANES]\n\t\tExample: ./ServerApp 5\n\n\t./ServerApp -s [SPEED] -l [NUMBER_OF_LANES]\n\t\tExample: ./server -s 50 -l 5\n\n")
#define MSG_SERVERAPP_SPEED_ARGUMENT_LIMIT _T("[INFO]: The speed value needs to be between 5 and 120")
#define MSG_SERVERAPP_NUMBER_OF_LANES_ARGUMENT_LIMIT _T("[INFO]: The number of lanes value needs to be between 1 and 8")
#define MSG_SERVERAPP_SPEED_KEY_FOUND _T("[INFO: Initial speed key was found!")
#define MSG_SERVERAPP_NUMBER_OF_LANES_KEY_FOUND _T("[INFO: Initial number of lanes key was found!")
#define MSG_SERVERAPP_SPEED_KEY_SET _T("[INFO: Initial speed key value was set!")
#define MSG_SERVERAPP_NUMBER_OF_LANES_KEY_SET _T("[INFO: Initial number of lanes key value was set!")
#define MSG_SERVERAPP_SPEED_KEY_OPENED _T("[INFO: Initial speed key was opened!")
#define MSG_SERVERAPP_NUMBER_OF_LANES_KEY_OPENED _T("[INFO: Initial number of lanes key was opened!")
#define MSG_SERVERAPP_SPEED_KEY_CREATED _T("[INFO: Initial speed key was created!")
#define MSG_SERVERAPP_NUMBER_OF_LANES_KEY_CREATED _T("[INFO: Initial number of lanes key was created!")



//ERROR MESSAGES
#define MSG_SERVERAPP_MULTIPLE_INSTANCES_RUNNING _T("[ERROR]: There's already a ServerApp instance running!")
#define MSG_SERVERAPP_UNKNOWN_ARGUMENT TEXT("[ERROR]: There's an unknown value in the arguments!")
#define MSG_SERVERAPP_INVALID_ARGUMENTS_QUANTITY _T("[ERROR]: There's an invalid number of arguments!")
#define MSG_SERVERAPP_INVALID_NUMBER_OF_LANES TEXT("[ERROR]: The number of lanes value is invalid!")
#define MSG_SERVERAPP_INVALID_SPEED TEXT("[ERROR]: The speed value is invalid!")
#define MSG_SERVERAPP_INVALID_KEY_NUMBER_OF_LANES TEXT("[ERROR]: There's no number of lanes value in the Registry!")
#define MSG_SERVERAPP_INVALID_KEY_SPEED TEXT("[ERROR]: There's no speed value in the Registry!")
#define MSG_SERVERAPP_CANT_OPEN_KEY TEXT("[ERROR]: Can't open the Key!")
#define MSG_SERVERAPP_CANT_CREATE_KEY TEXT("[ERROR]: Can't create the Key!")
#define MSG_SERVERAPP_CANT_SET_KEY TEXT("[ERROR]: Can't set the Key!")

//ERRORS
#define SUCCESS 0
#define ERROR_MULTIPLE_INSTANCES_RUNNING 1
#define ERROR_UNKNOWN_ARGUMENT 2
#define ERROR_INVALID_ARGUMENTS_QUANTITY 3
#define ERROR_INVALID_NUMBER_OF_LANES 4
#define ERROR_INVALID_SPEED 5
#define ERROR_INVALID_KEY_SPEED 6
#define ERROR_INVALID_KEY_NUMBER_OF_LANES 7
#define ERROR_CANT_OPEN_KEY 8
#define ERROR_CANT_CREATE_KEY 9
#define ERROR_CANT_SET_KEY 10



//MUTEX
#define FROGGER_SERVERAPP_ONLINE _T("FROGGER_SERVERAPP_ONLINE")

//SPEED
#define SPEED_STR TEXT("-s")
#define SPEED_MIN 5
#define SPEED_MAX 120

//NUMBER OF LANES
#define NUMBER_OF_LANES_STR TEXT("-l")
#define NUMBER_OF_LANES_MIN 1
#define NUMBER_OF_LANES_MAX 8

//REGISTRY
#define KEYPATH_NAME TEXT("SOFTWARE\\Frogger\\")
#define SPEED_KEY_NAME TEXT("SPEED")
#define NUMBER_OF_LANES_KEY_NAME TEXT("NUMBEROFLANES")

//LOGIC
#define TRUE 1
#define FALSE 0