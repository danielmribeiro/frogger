#include "Client.h"

ClientData cData;
HWND hWnd, g_hWnd = NULL;
TCHAR szProgName[] = TEXT("Frogger");
ClientCommsData communicationData = { 0 };
HANDLE clientGameThread = NULL;

DWORD WINAPI communicationHandlerProc(LPVOID* p) {
	ClientCommsData* c = (ClientCommsData*)p;
	while (cData.g.exit == 0) {
		ReadFile(communicationData.hServerPipe,
			&(cData.g),
			sizeof(GameInfo),
			NULL,
			NULL);

		PostMessage(hWnd, WM_GAME_UPDATE, 0, 0);
	}


	return 0;
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HWND hUsernameTextbox = NULL;
	TCHAR username[10];
	DWORD typeRequest = 0, response = 0;

	switch (messg)
	{
	case WM_GAME_UPDATE:
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;
	case WM_CREATE:
	{
		cData.screen = MENU;
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
				if (cData.g.frogs[0].pos.x > 0)
				{
					cData.g.frogs[0].pos.x--;
				}
				// TODO communicate
				break;
			case VK_RIGHT:
				if (cData.g.frogs[0].pos.x < 20 - 1)
				{
					cData.g.frogs[0].pos.x++;
				}
				// TODO communicate
				break;
			case VK_UP:
				if (cData.g.frogs[0].pos.y > 0)
				{
					cData.g.frogs[0].pos.y--;
				}
				// TODO communicate
				break;
			case VK_DOWN:
				if (cData.g.frogs[0].pos.y < cData.g.lanes - 1)
				{
					cData.g.frogs[0].pos.y++;
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
				clientGameThread = CreateThread(
					NULL,
					0,
					communicationHandlerProc,
					&communicationData,
					0,
					&clientGameThread
				);
				if (clientGameThread == NULL) {
					_tprintf(_T("error creating game thread\n"));
					typeRequest = CLIENT_SHUTDOWN;
					WriteFile(communicationData.hServerPipe,
						&typeRequest,
						sizeof(typeRequest),
						NULL,
						NULL);
					PostQuitMessage(0);
				}

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


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG lpMsg = { 0 };
	WNDCLASSEX wcApp;

	// TODO Check if the server is running
	if (!isServerRunning(&communicationData))
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
		NULL);

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
