#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../src/Base.h"
#include "../src/Constants.h"
#include "framework.h"
#include "Resource.h"
#include "ClientComms.h"

#define WM_GAME_UPDATE (WM_USER + 1)

typedef enum {
	WELCOME,
	MENU,
	INDIVIDUAL_WAIT,
	INDIVIDUAL_GAME,
	INDIVIDUAL_GAMEOVER,
	COMPETITIVE_WAIT,
	COMPETITIVE_GAME,
	COMPETITIVE_GAMEOVER
} ClientScreen;


typedef struct {
	ClientScreen screen;
	int currentBitmap;
	GameInfo g;
}ClientData;

#define WHITE RGB(255,255,255)
#define BLACK RGB(0,0,0)
#define RED RGB(255,0,0)
#define DARK_GREEN RGB(48, 104, 68)
#define GOLD RGB(255,215,0)
#define GRAY RGB(192, 192, 192)
#define DARK_BROWN RGB(58, 49, 47)
#define GREEN RGB(48, 104, 68)
#define BROWN RGB(139, 69, 19)

#define FONT_COURIER_NEW _T("Courier New")
#define FONT_ARIAL _T("Arial")
#define FONT_TIMES_NEW_ROMAN _T("Times New Roman")
#define FONT_VERDANA _T("Verdana")
#define FONT_COMIC_SANS_MS _T("Comic Sans MS")

//ELEMENTS
void DrawBackgroundColor(HDC hdc, HWND hWnd, COLORREF color);
void DrawString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, const TCHAR* text, COLORREF textColor, const TCHAR* font);
void DrawInt(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, int number, COLORREF textColor, const TCHAR* font);
void DrawTextbox(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int height, int width, int fontSize, const TCHAR* text, const TCHAR* font);
void DrawButton(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int height, int width, int fontSize, const TCHAR* text, int idc, const TCHAR* font);
void DrawWinnerString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, ClientData* cData, int gamemode, COLORREF textColor, const TCHAR* font);
void DrawFrog(HDC hdc, HWND hWnd, int bitmapX, int bitmapY, int currentBitmap, int frogID);
void DrawRoads(HDC hdc, int numRoads);
void DrawMapElement(HDC hdc, HWND hWnd, int elementType, int posX, int posY, ClientData* cData);
void DrawMap(HDC hdc, HWND hWnd, ClientData* cData);


//SCREEN
void PaintScreenWelcome(HDC hdc, HWND hWnd);
void PaintScreenMenu(HDC hdc, HWND hWnd);
void PaintScreenIndividualWait(HDC hdc, HWND hWnd);
void PaintScreenIndividualGame(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenIndividualGameover(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenCompetitiveWait(HDC hdc, HWND hWnd);
void PaintScreenCompetitiveGame(HDC hdc, HWND hWnd, ClientData* cData);
void PaintScreenCompetitiveGameover(HDC hdc, HWND hWnd, ClientData* cData);


#define IDC_COMPETITIVE_BUTTON 1001
#define IDC_INDIVIDUAL_BUTTON 1002
#define IDC_USERNAME_TEXTBOX 1003
#define IDC_MENU_BUTTON 1004
#define IDC_BITMAP_BUTTON 1005

#endif