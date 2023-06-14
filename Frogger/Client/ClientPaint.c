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


void DrawWinnerString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, ClientData* cData, int gamemode, COLORREF textColor, const TCHAR* font) {
	TCHAR str1[256];
	TCHAR str2[256];

	if (gamemode == 0) {
		_tcscpy_s(str1, 256, _T("Well Done "));
		_tcscpy_s(str2, 256, cData->frog[0].username);
	}
	else if (gamemode == 1) {
		if (cData->frog[0].score > cData->frog[1].score) {
			_tcscpy_s(str1, 256, cData->frog[0].username);
			_tcscpy_s(str2, 256, _T(" Wins!"));
		}
		else if(cData->frog[0].score < cData->frog[1].score) {
			_tcscpy_s(str1, 256, cData->frog[1].username);
			_tcscpy_s(str2, 256, _T(" Wins!"));
		}
		else {
			_tcscpy_s(str1, 256, _T(""));
			_tcscpy_s(str2, 256, _T("It's a tie!"));
		}
	}
	// Calculate the required size for the concatenation
	int size = _tcslen(str1) + _tcslen(str2) + 1;

	// Allocate memory for the concatenation
	TCHAR* result = (TCHAR*)malloc(size * sizeof(TCHAR));

	if (result != NULL) {
		// Copy the first string to the result
		_tcscpy_s(result, size, str1);

		// Concatenate the second string with the result
		_tcscat_s(result, size, str2);

		DrawString(hdc, hWnd, xStrPos, yStrPos, fontSize, result, textColor, font);
	}
	else {
		printf("Failed to allocate memory.\n");
	}
}

void DrawFrog(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int currentBitmap, int frogID) {
	HBITMAP hBitmap;
	switch (currentBitmap) {
	case 0:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	case 1:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	default:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	}
	
	if (hBitmap != NULL) {
		// Create a compatible device context
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			// Select the bitmap into the device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

			// Get the size of the bitmap
			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			// Draw the bitmap onto the device context
			BitBlt(hdc, xStrPos, yStrPos, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
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

void PaintScreenIndividualGameover(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -200, 50, TEXT("GAMEOVER"), RED, FONT_ARIAL);
	DrawWinnerString(hdc, hWnd, 0, -100, 20, cData, 0, GOLD, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 30, 300, cData->currentBitmap,0);
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

	// Call the function to draw the roads
	//DrawRoads(hdc, cData->nRoads); // Change the number of roads here

	//DrawFrogMap(hdc, cData->nRoads); //DrawFrogs

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

void PaintScreenCompetitiveGameover(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -200, 50, TEXT("GAMEOVER"), RED, FONT_ARIAL);
	DrawWinnerString(hdc, hWnd, 0, -100, 20, cData, 1, GOLD, FONT_COMIC_SANS_MS);
	//Drawfrog1
	//DrawString username1
	//DrawString score1
	//Drawfrog2
	//DrawString username2
	//DrawString score2
	//GoToMenuButton
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}
