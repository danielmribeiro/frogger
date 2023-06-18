#include "Client.h"

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
ClientData cData;
HWND g_hWnd = NULL;
TCHAR szProgName[] = TEXT("Frogger");
ClientCommsData communicationData = { 0 };
GameInfo g;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg = { 0 };
	WNDCLASSEX wcApp;

	// TODO Check if the server is running
	if (!isServerRunning(&communicationData, &g))
	{
		return -1;
	}

	// TODO CreateThread for communication with server

	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;
	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = TrataEventos;
	wcApp.style = CS_HREDRAW | CS_VREDRAW;
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcApp.lpszMenuName = NULL;
	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (!RegisterClassEx(&wcApp))
		return (0);

	hWnd = CreateWindow(
		szProgName,
		TEXT("Frogger"),
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		635,
		635,
		(HWND)HWND_DESKTOP,
		(HMENU)NULL,
		(HINSTANCE)hInst,
		&g);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}

	WaitForSingleObject(communicationData.hCommunicationThread, INFINITE);
	DisconnectNamedPipe(communicationData.hServerPipe);
	CloseHandle(communicationData.hServerPipe);
	return ((int)lpMsg.wParam);
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	GameInfo* g = NULL;
	HWND hUsernameTextbox = NULL;
	TCHAR username[10];
	DWORD typeRequest = 0, response = 0;

	if (messg != WM_CREATE)
		g = (GameInfo*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (messg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		g = (GameInfo*)cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)g);

		cData.screen = MENU;
		/*
		cData.nRoads = 10;
		cData.level = 1;
		cData.currentBitmap = 1;
		cData.currentBitmap = 0;
		cData.frog[0].pos.x = 1;
		cData.frog[0].pos.y = 1;
		cData.frog[0].score = 50;
		cData.frog[0].username = TEXT("Player 1");
		cData.frog[1].pos.x = 2;
		cData.frog[1].pos.y = 2;
		cData.frog[1].score = 60;
		cData.frog[1].username = TEXT("Player 2");
		cData.car[0][0].dir = LEFT;
		cData.car[0][0].pos.x = 3;
		cData.car[0][0].pos.y = 3;
		cData.car[0][1].dir = LEFT;
		cData.car[0][1].pos.x = 4;
		cData.car[0][1].pos.y = 4;
		cData.obstacle[0][0].pos.x = 5;
		cData.obstacle[0][0].pos.y = 5;
		cData.obstacle[0][1].pos.x = 6;
		*/
		break;
	}
	case WM_CLOSE:
	case WM_DESTROY:
		if (MessageBox(hWnd, TEXT("Do you want to exit?"), TEXT("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			DWORD requestType = CLIENT_SHUTDOWN;
			WriteFile(communicationData.hServerPipe,
				&requestType,
				sizeof(requestType),
				NULL,
				NULL);
			PostQuitMessage(0);
		}
		break;
	case WM_KEYDOWN:
	{
		if (cData.screen == COMPETITIVE_GAME || cData.screen == INDIVIDUAL_GAME)
		{
			int key = wParam;
			switch (key)
			{
			case VK_LEFT:
				if (cData.frog[0].pos.x > 0)
				{
					cData.frog[0].pos.x--;
				}
				// TODO communicate
				break;
			case VK_RIGHT:
				if (cData.frog[0].pos.x < 20 - 1)
				{
					cData.frog[0].pos.x++;
				}
				// TODO communicate
				break;
			case VK_UP:
				if (cData.frog[0].pos.y > 0)
				{
					cData.frog[0].pos.y--;
				}
				// TODO communicate
				break;
			case VK_DOWN:
				if (cData.frog[0].pos.y < cData.nRoads - 1)
				{
					cData.frog[0].pos.y++;
				}
				// TODO communicate
				break;
			default:
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_COMPETITIVE_BUTTON:
		{
			hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			GetWindowText(hUsernameTextbox, username, sizeof(username) / sizeof(username[0]));
			cData.screen = COMPETITIVE_WAIT;

			HWND hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			HWND hCompetitiveButton = GetDlgItem(hWnd, IDC_COMPETITIVE_BUTTON);
			HWND hIndividualButton = GetDlgItem(hWnd, IDC_INDIVIDUAL_BUTTON);

			DestroyWindow(hUsernameTextbox);
			DestroyWindow(hCompetitiveButton);
			DestroyWindow(hIndividualButton);

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		case IDC_INDIVIDUAL_BUTTON:
		{
			hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			GetWindowText(hUsernameTextbox, username, sizeof(username) / sizeof(username[0]));
			cData.screen = INDIVIDUAL_WAIT;

			HWND hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			HWND hCompetitiveButton = GetDlgItem(hWnd, IDC_COMPETITIVE_BUTTON);
			HWND hIndividualButton = GetDlgItem(hWnd, IDC_INDIVIDUAL_BUTTON);

			DestroyWindow(hUsernameTextbox);
			DestroyWindow(hCompetitiveButton);
			DestroyWindow(hIndividualButton);

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);

			// TODO send message to server that individual game is starting
			typeRequest = PLAY_INDIVIDUAL;
			WriteFile(communicationData.hServerPipe,
				&typeRequest,
				sizeof(typeRequest),
				NULL,
				NULL);

			ReadFile(communicationData.hServerPipe,
				&response,
				sizeof(response),
				NULL,
				NULL);

			// TODO createThread to read from game
			if (response)
			{
				cData.screen = INDIVIDUAL_GAME;
			}
			else {
				cData.screen = MENU;
			}

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		case IDC_MENU_BUTTON:
		{
			cData.screen = MENU;
			HWND hMenuButton = GetDlgItem(hWnd, IDC_MENU_BUTTON);
			DestroyWindow(hMenuButton);

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		case IDC_BITMAP_BUTTON:
		{
			if (cData.currentBitmap == 0)
			{
				cData.currentBitmap = 1;
			}
			else
			{
				cData.currentBitmap = 0;
			}

			HWND hBitmapButton = GetDlgItem(hWnd, IDC_BITMAP_BUTTON);
			DestroyWindow(hBitmapButton);

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		switch (cData.screen)
		{
		case WELCOME:
			PaintScreenWelcome(hdc, hWnd);
			break;
		case MENU:
			PaintScreenMenu(hdc, hWnd);
			break;
		case INDIVIDUAL_WAIT:
			PaintScreenIndividualWait(hdc, hWnd);
			break;
		case INDIVIDUAL_GAME:
			PaintScreenIndividualGame(hdc, hWnd, &cData);
			break;
		case INDIVIDUAL_GAMEOVER:
			PaintScreenIndividualGameover(hdc, hWnd, &cData);
			break;
		case COMPETITIVE_WAIT:
			PaintScreenCompetitiveWait(hdc, hWnd);
			break;
		case COMPETITIVE_GAME:
			PaintScreenCompetitiveGame(hdc, hWnd, &cData);
			break;
		case COMPETITIVE_GAMEOVER:
			PaintScreenCompetitiveGameover(hdc, hWnd, &cData);
			break;
		default:
			break;
		}

		EndPaint(hWnd, &ps);
		break;
	}
	default:
		return (DefWindowProc(hWnd, messg, wParam, lParam));
	}

	return 0;
}