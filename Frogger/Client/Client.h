#pragma once

#include "resource.h"
#include <windows.h>
#include <tchar.h>

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
	ClientScreen screen;
	int nRoads;
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