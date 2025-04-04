#include "Client.h"

void DrawBackgroundColor(HDC hdc, HWND hWnd, COLORREF color) {
	HBRUSH hBrush = CreateSolidBrush(color);
	RECT rect;
	GetClientRect(hWnd, &rect);
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

void DrawString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, const TCHAR* text, COLORREF textColor, const TCHAR* font) {
	HFONT hFont = CreateFont(-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font);

	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	RECT rect;
	GetClientRect(hWnd, &rect);
	int textWidth = rect.right - rect.left;
	int textHeight = rect.bottom - rect.top;

	SIZE textSize;
	GetTextExtentPoint32(hdc, text, lstrlen(text), &textSize);

	int xPos = (textWidth - textSize.cx) / 2 + xStrPos;
	int yPos = (textHeight - textSize.cy) / 2 + yStrPos;

	SetTextColor(hdc, textColor);

	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, xPos, yPos, text, lstrlen(text));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawInt(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, int number, COLORREF textColor, const TCHAR* font) {
	TCHAR* text[256];
	swprintf_s(text, _countof(text), _T("%d"), number);
	DrawString(hdc, hWnd, xStrPos, yStrPos, fontSize, text, textColor, font);
}

void DrawTextbox(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int height, int width, int fontSize, const TCHAR* text, const TCHAR* font) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	int screenWidth = rect.right - rect.left;
	int screenWeight = rect.bottom - rect.top;
	int centerX = screenWidth / 2;
	int centerY = screenWeight / 2;

	HFONT hFontUsername = CreateFont(-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		VARIABLE_PITCH, font);
	SelectObject(hdc, hFontUsername);

	rect.top = centerY + yStrPos;
	rect.bottom = rect.top + height;
	rect.left = centerX + xStrPos;
	rect.right = rect.left + width;
	HWND hUsernameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), text,
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		hWnd, (HMENU)IDC_USERNAME_TEXTBOX, GetModuleHandle(NULL), NULL);
	SendMessage(hUsernameTextbox, WM_SETFONT, (WPARAM)hFontUsername, TRUE);
}

void DrawButton(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int height, int width, int fontSize, const TCHAR* text, int idc, const TCHAR* font) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	int screenWidth = rect.right - rect.left;
	int screenWeight = rect.bottom - rect.top;
	int centerX = screenWidth / 2;
	int centerY = screenWeight / 2;

	HFONT hFontButton = CreateFont(-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		VARIABLE_PITCH, font);
	SelectObject(hdc, hFontButton);

	rect.top = centerY + yStrPos;
	rect.bottom = rect.top + height;
	rect.left = centerX + xStrPos;
	rect.right = rect.left + width;
	HWND hButton = CreateWindow(TEXT("BUTTON"), text,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		hWnd, (HMENU)idc, GetModuleHandle(NULL), NULL);
	SendMessage(hButton, WM_SETFONT, (WPARAM)hFontButton, TRUE);
}

void DrawWinnerString(HDC hdc, HWND hWnd, int xStrPos, int yStrPos, int fontSize, ClientData* cData, int gamemode, COLORREF textColor, const TCHAR* font) {
	TCHAR str1[256];
	TCHAR str2[256];

	if (gamemode == 0) {
		_tcscpy_s(str1, 256, _T("Well Done "));
		_tcscpy_s(str2, 256, cData->g.frogs[0].username);
	}
	else if (gamemode == 1) {
		if (cData->g.frogs[0].score > cData->g.frogs[1].score) {
			_tcscpy_s(str1, 256, cData->g.frogs[0].username);
			_tcscpy_s(str2, 256, _T(" Wins!"));
		}
		else if (cData->g.frogs[0].score < cData->g.frogs[1].score) {
			_tcscpy_s(str1, 256, cData->g.frogs[1].username);
			_tcscpy_s(str2, 256, _T(" Wins!"));
		}
		else {
			_tcscpy_s(str1, 256, _T(""));
			_tcscpy_s(str2, 256, _T("It's a tie!"));
		}
	}
	int size = _tcslen(str1) + _tcslen(str2) + 1;

	TCHAR* result = (TCHAR*)malloc(size * sizeof(TCHAR));

	if (result != NULL) {
		_tcscpy_s(result, size, str1);

		_tcscat_s(result, size, str2);

		DrawString(hdc, hWnd, xStrPos, yStrPos, fontSize, result, textColor, font);
	}
	else {
		printf("Failed to allocate memory.\n");
	}
}

void DrawFrog(HDC hdc, HWND hWnd, int bitmapX, int bitmapY, int currentBitmap, int frogID) {
	HBITMAP hBitmap;
	switch (currentBitmap) {
	case 0:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	case 1:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	default:
		if (frogID == 0) {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		else {
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		break;
	}

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

	BITMAP bitmapInfo;
	GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);
	int bitmapWidth = bitmapInfo.bmWidth;
	int bitmapHeight = bitmapInfo.bmHeight;

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int centerX = (clientRect.right - clientRect.left) / 2;
	int centerY = (clientRect.bottom - clientRect.top) / 2;
	int bitmapLeft = centerX - bitmapWidth / 2 + bitmapX;
	int bitmapTop = centerY - bitmapHeight / 2 + bitmapY;

	BitBlt(hdc, bitmapLeft, bitmapTop, bitmapWidth, bitmapHeight, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);
}

void DrawRoads(HDC hdc, int numRoads) {
	int startX = 10;
	int startY = 10;
	int mapHeight = 300;
	int mapWidth = 600;
	int roadSpacing = 30;
	int totalRoads = 10;

	for (int i = 0; i < totalRoads; i++) {
		int roadTop = startY + i * roadSpacing;
		int roadBottom = roadTop + roadSpacing - 1;

		COLORREF roadColor;
		if (i == 0 || i >= numRoads - 1) {
			roadColor = GREEN;
		}
		else {
			roadColor = DARK_BROWN;
		}

		RECT roadRect = { startX, roadTop, startX + mapWidth, roadBottom };
		HBRUSH hBrush = CreateSolidBrush(roadColor);
		FillRect(hdc, &roadRect, hBrush);
		DeleteObject(hBrush);

		if (i != numRoads - 1) {
			HPEN hPen = CreatePen(PS_SOLID, 4, GRAY);
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, startX, roadBottom, NULL);
			LineTo(hdc, startX + mapWidth, roadBottom);
			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
		}

		HPEN hBrownPen = CreatePen(PS_SOLID, 1, BROWN);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hBrownPen);

		int numVerticalLines = 19;
		int lineSpacing = mapWidth / (numVerticalLines + 1);
		int lineX = startX + lineSpacing;

		for (int j = 0; j < numVerticalLines; j++) {
			MoveToEx(hdc, lineX, roadTop, NULL);
			LineTo(hdc, lineX, roadBottom);
			lineX += lineSpacing;
		}

		SelectObject(hdc, hOldPen);
		DeleteObject(hBrownPen);
	}
	if (numRoads < totalRoads) {

	}

}

void DrawMapElement(HDC hdc, HWND hWnd, int elementType, int posX, int posY, ClientData* cData) {
	HBITMAP hBitmap = NULL;
	switch (elementType) {
	case 0: //Frog
		switch (cData->currentBitmap) {
		case 0:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case 1:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		default:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		}
		break;
	case 1: //OpponentFrog
		switch (cData->currentBitmap) {
		case 0:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case 1:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		default:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		}
		break;
	case 2: //Car
		switch (cData->currentBitmap) {
		case 0:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Car1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case 1:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Car2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		default:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Car1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		}
		break;
	case 3: //Obstacle
		switch (cData->currentBitmap) {
		case 0:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Obstacle1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case 1:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Obstacle2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		default:
			hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Obstacle1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		}
		break;
	default:
		break;
	}

	if (hBitmap != NULL) {
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

			int obstacleX = (posX * 30) + 10;
			int obstacleY = (posY * 30) + 10;

			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			BitBlt(hdc, obstacleX, obstacleY, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}

void DrawMap(HDC hdc, HWND hWnd, int gameMode, ClientData* cData) {
	DrawRoads(hdc, cData->g.lanes);

	if (gameMode == 0) {
		DrawMapElement(hdc, hWnd, 0, cData->g.frogs[0].pos.x, cData->g.frogs[0].pos.y, cData);
	}
	else if (gameMode == 1) {
		DrawMapElement(hdc, hWnd, 0, cData->g.frogs[0].pos.x, cData->g.frogs[0].pos.y, cData);
		DrawMapElement(hdc, hWnd, 1, cData->g.frogs[1].pos.x, cData->g.frogs[1].pos.y, cData);
	}
	//////////////////TODO/////////FAZER CICLO DOS CARROS
	for (int i = 0; i < cData->g.lanes; i++) {
		for (int j = 0; j < cData->g.numCars[i]; j++)
			DrawMapElement(hdc, hWnd, 2, cData->g.cars[i][j].pos.x, cData->g.cars[i][j].pos.y, cData);

		for (int j = 0; j < cData->g.numObs[i]; j++)
			DrawMapElement(hdc, hWnd, 3, cData->g.obstacles[i][j].pos.x, cData->g.obstacles[i][j].pos.y, cData);

	}

}

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
	DrawTextbox(hdc, hWnd, -150, -100, 50, 300, 25, TEXT("Player"), FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 50, 20, TEXT("Gamemode:"), WHITE, FONT_ARIAL);
	DrawButton(hdc, hWnd, -275, 100, 50, 250, 20, TEXT("INDIVIDUAL"), IDC_INDIVIDUAL_BUTTON, FONT_ARIAL);
	DrawButton(hdc, hWnd, 25, 100, 50, 250, 20, TEXT("COMPETITIVE"), IDC_COMPETITIVE_BUTTON, FONT_ARIAL);
}

void PaintScreenIndividualWait(HDC hdc, HWND hWnd) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -100, 80, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	DrawString(hdc, hWnd, 0, 50, 40, TEXT("Loading..."), WHITE, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 100, 20, TEXT("Waiting for Individual Game..."), WHITE, FONT_ARIAL);
}

void PaintScreenIndividualGame(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, BLACK);
	DrawMap(hdc, hWnd, 0, cData);
	DrawInt(hdc, hWnd, 0, 100, 20, cData->g.level, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 50, 100, 20, cData->g.time, RED, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 150, cData->currentBitmap, 0);
	DrawString(hdc, hWnd, -100, 150, 20, cData->g.frogs[0].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 150, 20, cData->g.frogs[0].score, RED, FONT_COMIC_SANS_MS);
	DrawButton(hdc, hWnd, 175, 50, 50, 100, 10, TEXT("BITMAP"), IDC_BITMAP_BUTTON, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	//hoverfrog1

}

void PaintScreenIndividualGameover(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -200, 50, TEXT("GAMEOVER"), RED, FONT_ARIAL);
	DrawWinnerString(hdc, hWnd, 0, -100, 20, cData, 0, GOLD, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 0, cData->currentBitmap, 0);
	DrawString(hdc, hWnd, -100, 0, 20, cData->g.frogs[0].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 0, 20, cData->g.frogs[0].score, RED, FONT_COMIC_SANS_MS);
	DrawButton(hdc, hWnd, -100, 100, 50, 200, 20, TEXT("MENU"), IDC_MENU_BUTTON, FONT_ARIAL);
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
	DrawMap(hdc, hWnd, 1, cData);
	DrawInt(hdc, hWnd, 0, 100, 20, cData->g.level, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 50, 100, 20, cData->g.time, RED, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 150, cData->currentBitmap, 0);
	DrawString(hdc, hWnd, -100, 150, 20, cData->g.frogs[0].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 150, 20, cData->g.frogs[0].score, RED, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 200, cData->currentBitmap, 1);
	DrawString(hdc, hWnd, -100, 200, 20, cData->g.frogs[1].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 200, 20, cData->g.frogs[1].score, RED, FONT_COMIC_SANS_MS);
	DrawButton(hdc, hWnd, 175, 50, 50, 100, 10, TEXT("BITMAP"), IDC_BITMAP_BUTTON, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
	//hoverfrog1
	//hoverfrog2
}

void PaintScreenCompetitiveGameover(HDC hdc, HWND hWnd, ClientData* cData) {
	DrawBackgroundColor(hdc, hWnd, DARK_GREEN);
	DrawString(hdc, hWnd, 0, -200, 50, TEXT("GAMEOVER"), RED, FONT_ARIAL);
	DrawWinnerString(hdc, hWnd, 0, -100, 20, cData, 1, GOLD, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 0, cData->currentBitmap, 0);
	DrawString(hdc, hWnd, -100, 0, 20, cData->g.frogs[0].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 0, 20, cData->g.frogs[0].score, RED, FONT_COMIC_SANS_MS);
	DrawFrog(hdc, hWnd, 0, 50, cData->currentBitmap, 1);
	DrawString(hdc, hWnd, -100, 50, 20, cData->g.frogs[1].username, RED, FONT_COMIC_SANS_MS);
	DrawInt(hdc, hWnd, 100, 50, 20, cData->g.frogs[1].score, RED, FONT_COMIC_SANS_MS);
	DrawButton(hdc, hWnd, -100, 100, 50, 200, 20, TEXT("MENU"), IDC_MENU_BUTTON, FONT_ARIAL);
	DrawString(hdc, hWnd, 0, 250, 20, TEXT("FROGGER"), RED, FONT_COMIC_SANS_MS);
}
