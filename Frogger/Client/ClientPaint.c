#include "Client.h"

void DrawBackgroundColor(HDC hdc, HWND hWnd, COLORREF color) {
	// Create a "color" brush
	HBRUSH hBrush = CreateSolidBrush(color);

	// Get the client area rectangle
	RECT rect;
	GetClientRect(hWnd, &rect);

	// Fill the client area with the green brush
	FillRect(hdc, &rect, hBrush);

	DeleteObject(hBrush);
}

void DrawString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, const TCHAR* text, COLORREF textColor, const TCHAR* font) {
	// Create a font with the specified size
	HFONT hFont = CreateFont(
		-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72),
		0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font
	);

	// Select the font into the DC
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	// Get the text dimensions
	RECT rect;
	GetClientRect(hWnd, &rect);
	int textWidth = rect.right - rect.left;
	int textHeight = rect.bottom - rect.top;

	SIZE textSize;
	GetTextExtentPoint32(hdc, text, lstrlen(text), &textSize);

	// Calculate the position to align the text to the middle of the screen
	int xPos = (textWidth - textSize.cx) / 2 + xStrPos;
	int yPos = (textHeight - textSize.cy) / 2 + yStrPos;

	// Set the text color
	SetTextColor(hdc, textColor);

	// Remove the white background of the text
	SetBkMode(hdc, TRANSPARENT);

	// Print the string
	TextOut(hdc, xPos, yPos, text, lstrlen(text));

	// Restore the original font and clean up
	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}




//SCREEN
void PaintScreenWelcome(HDC hdc, HWND hWnd) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -100, 80, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	DrawString(hdc, hWnd, 0, 50, 40, TEXT("Loading..."), WHITE, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 100, 20, TEXT("Connecting to the server..."), WHITE, FONT_ARIAL);
}

void PaintScreenMenu(HDC hdc, HWND hWnd) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -250, 30, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	DrawString(hdc, hWnd, 0, -150, 20, TEXT("Username:"), WHITE, FONT_ARIAL);
	//DrawStringUsername
	//DrawTextboxUsername
	//if(error)
	//DrawStringInvalidUsername
	DrawString(hdc, hWnd, 0, 100, 30, TEXT("Gamemode"), WHITE, FONT_ARIAL);
	//DrawButtonCompetitive
	//DrawButtonIndividual
	//DrawStringGameModeSelected
	//DrawButtonNext
}

void PaintScreenIndividualWait(HDC hdc, HWND hWnd) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -100, 80, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	DrawString(hdc, hWnd, 0, 50, 40, TEXT("Loading..."), WHITE, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 100, 20, TEXT("Waiting for Individual Game..."), WHITE, FONT_ARIAL);
}

void PaintScreenIndividualGame(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, BLACK);
	//DrawMap
	//DrawFrog
	//for DrawCar
	//for DrawObstacle
	//DrawStringLevel
	//DrawFrog1
	//DrawStringPlayerName1
	//DrawStringPlayerScore1
	//DrawStringTime
	//bitmapButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	//hoverfrog1

}

void PaintScreenIndividualWin(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, GOLD);
	//DrawString you win
	//Drawfrog1
	//DrawString username1
	//DrawString score1
	//GoToMenuButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}

void PaintScreenIndividualLost(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, RED);
	//DrawString you lost
	//Drawfrog1
	//DrawString username1
	//DrawString score1
	//GoToMenuButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}

void PaintScreenCompetitiveWait(HDC hdc, HWND hWnd) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -100, 80, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	DrawString(hdc, hWnd, 0, 50, 40, TEXT("Loading..."), WHITE, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 100, 20, TEXT("Searching for opponent..."), WHITE, FONT_ARIAL);
}

void PaintScreenCompetitiveGame(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, BLACK);
	//DrawMap
	//DrawFrog
	//DrawFrogOpponent
	//for DrawCar
	//for DrawObstacle
	//DrawStringLevel
	//DrawFrog1
	//DrawFrog2
	//DrawStringPlayerName1
	//DrawStringPlayerName2
	//DrawStringPlayerScore1
	//DrawStringPlayerScore2
	//DrawStringTime
	//bitmapButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	//hoverfrog1
	//hoverfrog2

	//-----------------------DELETE THIS-:-----------------------------------

	//// Call the function to draw the roads
	//DrawRoads(hdc, cData->nRoads); // Change the number of roads here

	//DrawFrog(hdc, cData->nRoads); //DrawFrogs

	//DrawCar(hdc, cData->nRoads, 1, 5); // Draw car
	//DrawCar(hdc, cData->nRoads, 2, 3); // Draw car
	//DrawCar(hdc, cData->nRoads, 3, 14); // Draw car
	//DrawCar(hdc, cData->nRoads, 3, 15); // Draw car
	//DrawCar(hdc, cData->nRoads, 3, 16); // Draw car
	//DrawCar(hdc, cData->nRoads, 4, 10); // Draw car
	//DrawCar(hdc, cData->nRoads, 5, 6); // Draw car
	//DrawCar(hdc, cData->nRoads, 6, 2); // Draw car
	//DrawCar(hdc, cData->nRoads, 6, 3); // Draw car
	//DrawCar(hdc, cData->nRoads, 7, 17); // Draw car
	//DrawCar(hdc, cData->nRoads, 8, 13); // Draw car

	//DrawObstacle(hdc, cData->nRoads, 3, 13);
	//DrawObstacle(hdc, cData->nRoads, 6, 4);

	//DrawOpponent(hdc, cData->nRoads, 6, 7);

	//DrawStrTitle(hdc);

	//TCHAR level[] = TEXT("Level 1");
	//DrawStrLevel(hdc, level);

	//TCHAR playerName[] = TEXT("Player1");
	//DrawStrPlayerName(hdc, 1, playerName);
	//TCHAR opponentName[] = TEXT("Player2");
	//DrawStrPlayerName(hdc, 2, opponentName);


	//TCHAR score[] = TEXT("Score: 50");
	//DrawStrScore(hdc, 1, score);
	//DrawStrScore(hdc, 2, score);

	//TCHAR time[] = TEXT("00:30");
	//DrawStrTime(hdc, time);

	//-------------END DELETE---------------------
}

void PaintScreenCompetitiveWin(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, GOLD);
	//DrawString Winner + username
	//DrawString you win
	//Drawfrog1
	//DrawString username1
	//DrawString score1
	//Drawfrog2
	//DrawString username2
	//DrawString score2
	//GoToMenuButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}

void PaintScreenCompetitiveLost(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, RED);
	//DrawString Winner + username
	//DrawString you lost
	//Drawfrog1
	//DrawString username1
	//DrawString score1
	//Drawfrog2
	//DrawString username2
	//DrawString score2
	//GoToMenuButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}
