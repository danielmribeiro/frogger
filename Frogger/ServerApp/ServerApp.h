#pragma once
#include <Windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//TITLE MESSAGES
#define MSG_SERVERAPP_TITLE_FROGGER _T("\n::::::: FROGGER - ServerApp :::::::")
#define MSG_SERVERAPP_TITLE_CHECK_SINGLE_INSTANCE _T("\n------- CHECK SINGLE INSTANCE -------")
#define MSG_SERVERAPP_TITLE_SPEED _T("\n------- SPEED -------")
#define MSG_SERVERAPP_TITLE_SET_SPEED_FROM_ARGUMENTS _T("\n------- SET SPEED FROM ARGUMENTS -------")
#define MSG_SERVERAPP_TITLE_SET_SPEED_FROM_REGISTRY _T("\n------- SET SPEED FROM REGISTRY -------")
#define MSG_SERVERAPP_TITLE_SAVE_SPEED _T("\n------- SAVE SPEED VALUE -------")
#define MSG_SERVERAPP_TITLE_NUMBER_OF_LANES _T("\n------- NUMBER OF LANES -------")
#define MSG_SERVERAPP_TITLE_SET_NUMBER_OF_LANES_FROM_ARGUMENTS _T("\n------- SET NUMBER OF LANES FROM ARGUMENTS -------")
#define MSG_SERVERAPP_TITLE_SET_NUMBER_OF_LANES_FROM_REGISTRY _T("\n------- SET NUMBER OF LANES FROM REGISTRY -------")
#define MSG_SERVERAPP_TITLE_SAVE_NUMBER_OF_LANES _T("\n------- SAVE NUMBER OF LANES -------")
#define MSG_SERVERAPP_TITLE_EXIT _T("\n------- EXIT FROGGER -------")





//INFO MESSAGES
#define MSG_SERVERAPP_INFO_SINGLE_INSTANCE_RUNNING _T("[INFO]: This ServerApp instance is the only one running currently!")

#define MSG_SERVERAPP_INFO_SPEED_KEY_OPENED _T("[INFO]: Speed key was opened!")
#define MSG_SERVERAPP_INFO_SPEED_KEY_CREATED _T("[INFO]: Speed key was created!")
#define MSG_SERVERAPP_INFO_SPEED_KEY_READ _T("[INFO]: Speed key was read!")
#define MSG_SERVERAPP_INFO_SPEED_KEY_CLOSED _T("[INFO]: Speed key was closed!")
#define MSG_SERVERAPP_INFO_SPEED_KEY_SET _T("[INFO]: The value for the Speed was saved in the key!")
#define MSG_SERVERAPP_INFO_SPEED_SET _T("[INFO]: The value for Speed was set!")

#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_OPENED _T("[INFO]: Number Of Lanes key was opened!")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CREATED _T("[INFO]: Number Of Lanes key was created!")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_READ _T("[INFO]: Number Of Lanes key was read!")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_CLOSED _T("[INFO]: Number Of Lanes key was closed!")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_KEY_SET _T("[INFO]: The value for the Number Of Lanes was saved in the key!")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_SET _T("[INFO]: The value for Number Of Lanes was set!")

#define MSG_SERVERAPP_INFO_ARGUMENT_FORMAT _T("[INFO]: You should use one of the following options:\n\t./ServerApp\n\t\tExample: ./ServerApp\n\n\t./ServerApp -s [SPEED]\n\t\tExample: ./ServerApp 50\n\n\t./ServerApp -l [NUMBER_OF_LANES]\n\t\tExample: ./ServerApp 5\n\n\t./ServerApp -s [SPEED] -l [NUMBER_OF_LANES]\n\t\tExample: ./server -s 50 -l 5\n\n\t./ServerApp -l [NUMBER_OF_LANES] -s [SPEED]\n\t\tExample: ./server -l 5 -s 50\n\n")
#define MSG_SERVERAPP_INFO_SPEED_ARGUMENT_LIMIT _T("[INFO]: The speed value needs to be between 5 and 120")
#define MSG_SERVERAPP_INFO_NUMBER_OF_LANES_ARGUMENT_LIMIT _T("[INFO]: The number of lanes value needs to be between 1 and 8")

#define MSG_SERVERAPP_INFO_CLOSE_SUCCESS _T("[INFO]: ServerApp is closing successfully!")





//ERROR MESSAGES
#define MSG_SERVERAPP_ERROR_MULTIPLE_INSTANCES_RUNNING _T("[ERROR]: There's already a ServerApp instance running!")
#define MSG_SERVERAPP_ERROR_UNKNOWN_ARGUMENT _T("[ERROR]: There's an unknown value in the arguments!")
#define MSG_SERVERAPP_ERROR_INVALID_ARGUMENTS_QUANTITY _T("[ERROR]: There's an invalid number of arguments!")
#define MSG_SERVERAPP_ERROR_INVALID_NUMBER_OF_LANES _T("[ERROR]: The number of lanes value is invalid!")
#define MSG_SERVERAPP_ERROR_INVALID_SPEED _T("[ERROR]: The speed value is invalid!")
#define MSG_SERVERAPP_ERROR_INVALID_KEY_SPEED _T("[ERROR]: There's no speed value in the Registry!")
#define MSG_SERVERAPP_ERROR_INVALID_KEY_NUMBER_OF_LANES _T("[ERROR]: There's no number of lanes value in the Registry!")
#define MSG_SERVERAPP_ERROR_CANT_OPEN_KEY _T("[ERROR]: Can't open the Key!")
#define MSG_SERVERAPP_ERROR_CANT_CREATE_KEY _T("[ERROR]: Can't create the Key!")
#define MSG_SERVERAPP_ERROR_CANT_SET_KEY _T("[ERROR]: Can't set the Key!")





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
#define KEYPATH_NAME _T("SOFTWARE\\Frogger\\")
#define SPEED_KEY_NAME _T("SPEED")
#define NUMBER_OF_LANES_KEY_NAME _T("NUMBEROFLANES")





//LOGIC
#define TRUE 1
#define FALSE 0