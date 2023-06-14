#pragma once

#include "resource.h"
#include <windows.h>
#include <tchar.h>

#define MAX_LANES 10
#define MAX_CARS 8
#define MAX_OBSTACLES 20

typedef enum {
	WELCOME,
	MENU,
	INDIVIDUAL_WAIT,
	INDIVIDUAL_GAME,
	INDIVIDUAL_WIN,
	INDIVIDUAL_LOST,
	COMPETITIVE_WAIT,
	COMPETITIVE_GAME,
	COMPETITIVE_WIN,
	COMPETITIVE_LOST
} ClientScreen;

typedef struct {
	int x, y;
} Position;

typedef enum {
	RIGHT,
	LEFT,
	FRONT,
	BACK
} Direction;

typedef struct {
	Position pos;
	Direction dir;
} Car;

typedef struct {
	Position pos;
} Obstacle;

typedef struct {
	Position pos;
	TCHAR* username;
	int score;
} Frog;

typedef struct {
	ClientScreen screen;
	int nRoads;
	int level;
	int currentBitmap;
	Frog frog[2];
	Car car[MAX_LANES][MAX_CARS];
	Obstacle obstacle[MAX_LANES][MAX_OBSTACLES];
	int time;
}ClientData;

#define WHITE RGB(255,255,255)
#define BLACK RGB(0,0,0)
#define RED RGB(255,0,0)
#define DARK_GREEN RGB(48, 104, 68)
#define GOLD RGB(255,215,0)

#define FONT_COURIER_NEW _T("Courier New")
#define FONT_ARIAL _T("Arial")
#define FONT_TIMES_NEW_ROMAN _T("Times New Roman")
#define FONT_VERDANA _T("Verdana")
#define FONT_COMIC_SANS_MS _T("Comic Sans MS")

//ELEMENTS
void DrawBackgroundColor(HDC hdc, HWND hWnd, COLORREF color);
void DrawString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, const TCHAR* text, COLORREF textColor, const TCHAR* font);

//SCREEN
void PaintScreenWelcome(HDC hdc, HWND hWnd);
void PaintScreenMenu(HDC hdc, HWND hWnd);
void PaintScreenIndividualWait(HDC hdc, HWND hWnd);
void PaintScreenIndividualGame(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenIndividualWin(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenIndividualLost(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenCompetitiveWait(HDC hdc, HWND hWnd);
void PaintScreenCompetitiveGame(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenCompetitiveWin(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenCompetitiveLost(HDC hdc, HWND hWnd, ClientData* cData);