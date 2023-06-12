#include <Windows.h>
#include <tchar.h>
#include "framework.h"
#include "Client.h"

/* ===================================================== */
/* Programa base (esqueleto) para aplicações Windows     */
/* ===================================================== */
// Modelo para programas Windows:
//  Composto por 2 funções: 
//	WinMain()     = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recepção de mensagens provenientes do Windows
//     TrataEventos()= Processamentos da janela (pode ter outro nome)
//			1) É chamada pelo Windows (callback) 
//			2) Executa código em função da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

// Nome da classe da janela (para programas de uma só janela, normalmente este nome é 
// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das 
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Frogger");

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as funções de processamento da janela)
// Parâmetros:
//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0 destinada a conter parâmetros para o programa 
//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em ShowWindow()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; // hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX é uma estrutura cujos membros servem para definir as características da classe da janela


// ============================================================================
// 1. Definição das características da janela "wcApp" 
//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Instância da janela actualmente exibida ("hInst" é parâmetro de WinMain e vem inicializada daí)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endereço da função de processamento da janela ("TrataEventos" foi declarada no início e encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW; // Estilo da janela: Fazer o redraw se for modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // "hIcon" = handler do ícon normal // "NULL" = Icon definido no Windows // "IDI_APPLICATION" Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // "hIconSm" = handler do ícon pequeno // "NULL" = Icon definido no Windows // "IDI_INFORMATION" Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato) // "NULL" = Forma definida no Windows // "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter (NULL = não tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);  // "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  "GetStockObject".Neste caso o fundo será branco

// ============================================================================
// 2. Registar a classe "wcApp" no Windows
// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);

// ============================================================================
// 3. Criar a janela
// ============================================================================
	hWnd = CreateWindow(
		szProgName,// Nome da janela (programa) definido acima
		TEXT("Frogger"), // Texto que figura na barra do título
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), // Estilo da janela (WS_OVERLAPPED= normal) // Remove resizable borders and maximize button
		CW_USEDEFAULT, // Posição x pixels (default=à direita da última)
		CW_USEDEFAULT, // Posição y pixels (default=abaixo da última)
		635, // Largura da janela (em pixels)
		635, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de outra) ou HWND_DESKTOP se a janela for a primeira, criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da instância do programa actual ("hInst" é passado num dos parâmetros de WinMain()
		0); // Não há parâmetros adicionais para a janela

// ============================================================================
// 4. Mostrar a janela
// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por "CreateWindow"; "nCmdShow"= modo de exibição (p.e. normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd); // Refrescar a janela (Windows envia à janela uma mensagem para pintar, mostrar dados, (refrescar)… 

// ============================================================================
// 5. Loop de Mensagens
// ============================================================================
// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de espera até que GetMessage(...) possa ler "a mensagem seguinte"	
// Parâmetros de "getMessage":
// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no início de WinMain()):
//			HWND hwnd		handler da janela a que se destina a mensagem
//			UINT message		Identificador da mensagem
//			WPARAM wParam		Parâmetro, p.e. código da tecla premida
//			LPARAM lParam		Parâmetro, p.e. se ALT também estava premida
//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
//			POINT pt		Localização do mouse (x, y) 
// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem receber as mensagens para todas as janelas pertencentes à thread actual)
// 3)Código limite inferior das mensagens que se pretendem receber
// 4)Código limite superior das mensagens que se pretendem receber

// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela, terminando então o loop de recepção de mensagens, e o programa 
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);// Pré-processamento da mensagem (p.e. obter código ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que aguarda até que a possa reenviar à função de tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

// ============================================================================
// 6. Fim do programa
// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o parâmetro wParam da estrutura lpMsg
}


void DrawRoads(HDC hdc, int numRoads) {
	// Calculate the height of each road
	int startX = 10;
	int startY = 10;
	int mapHeight = 300;//altura
	int mapWidth = 600; //largura
	int roadSpacing = 30;
	int totalRoads = 10;

	// Draw the roads
	for (int i = 0; i < totalRoads; i++) {
		// Calculate the coordinates for each road
		int roadTop = startY + i * roadSpacing;
		int roadBottom = roadTop + roadSpacing - 1;

		// Determine the road color
		COLORREF roadColor;
		if (i == 0 || i >= numRoads-1) {
			// First and last road color is green
			roadColor = RGB(48, 104, 68);
		}
		else {
			// Other roads color is black
			roadColor = RGB(58, 49, 47);
		}

		// Draw the road
		RECT roadRect = { startX, roadTop, startX + mapWidth, roadBottom };
		HBRUSH hBrush = CreateSolidBrush(roadColor);
		FillRect(hdc, &roadRect, hBrush);
		DeleteObject(hBrush);

		// Draw the gray line separating the roads
		if (i != numRoads - 1) {
			HPEN hPen = CreatePen(PS_SOLID, 4, RGB(192, 192, 192));
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, startX, roadBottom, NULL);
			LineTo(hdc, startX + mapWidth, roadBottom);
			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
		}

		/// Draw vertical lines within the road
		HPEN hBrownPen = CreatePen(PS_SOLID, 1, RGB(139, 69, 19));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hBrownPen);

		int numVerticalLines = 19;
		int lineSpacing = mapWidth / (numVerticalLines + 1);  // Add 1 for the last line
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

void DrawFrog(HDC hdc, int numRoads) {
	// Load the bitmap image
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Frog1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap != NULL) {
		// Create a compatible device context
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			// Select the bitmap into the device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

			int xCell = 5; //celula horizontal
			int yInitialCell = ((numRoads - 1) * 30) + 10;//celula vertical
			// Define the coordinates for the frog image
			int frogX = (xCell * 30) + 10;  // Change the X-coordinate here
			int frogY = yInitialCell;  // Change the Y-coordinate here

			// Get the size of the bitmap
			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			// Draw the bitmap onto the device context
			BitBlt(hdc, frogX, frogY, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}

void DrawCar(HDC hdc, int numRoads, int yCell, int xCell) {
	// Load the bitmap image
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Car1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap != NULL) {
		// Create a compatible device context
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			// Select the bitmap into the device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

			
			// Define the coordinates for the car image
			int carX = (xCell * 30) + 10;  // Change the X-coordinate here
			int carY = (yCell * 30) +10;  // Change the Y-coordinate here

			// Get the size of the bitmap
			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			// Draw the bitmap onto the device context
			BitBlt(hdc, carX, carY, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}

void DrawObstacle(HDC hdc, int numRoads, int yCell, int xCell) {
	// Load the bitmap image
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Obstacle1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap != NULL) {
		// Create a compatible device context
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			// Select the bitmap into the device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);


			// Define the coordinates for the obstacle image
			int obstacleX = (xCell * 30) + 10;  // Change the X-coordinate here
			int obstacleY = (yCell * 30) + 10;  // Change the Y-coordinate here

			// Get the size of the bitmap
			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			// Draw the bitmap onto the device context
			BitBlt(hdc, obstacleX, obstacleY, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}

void DrawOpponent(HDC hdc, int numRoads, int yCell, int xCell) {
	// Load the bitmap image
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Opponent1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap != NULL) {
		// Create a compatible device context
		HDC hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC != NULL) {
			// Select the bitmap into the device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);


			// Define the coordinates for the obstacle image
			int obstacleX = (xCell * 30) + 10;  // Change the X-coordinate here
			int obstacleY = (yCell * 30) + 10;  // Change the Y-coordinate here

			// Get the size of the bitmap
			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			// Draw the bitmap onto the device context
			BitBlt(hdc, obstacleX, obstacleY, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}
}

void DrawStrLevel(HDC hdc, TCHAR* level) {
	int x = 250;
	int y = 400;
	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkColor(hdc, RGB(0, 0, 0)); // Set background color to black

	HFONT hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _T("Courier New"));
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	TextOut(hdc, x, y, level, _tcslen(level));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawStrTime(HDC hdc, TCHAR* time) {
	int x = 265;
	int y = 430;
	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkColor(hdc, RGB(0, 0, 0)); // Set background color to black

	HFONT hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _T("Courier New"));
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	TextOut(hdc, x, y, time, _tcslen(time));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawStrTitle(HDC hdc) {
	const TCHAR* text = _T("FROGGER");
	int x = 640/2/2;
	int y = 330;
	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkColor(hdc, RGB(0, 0, 0)); // Set background color to black

	HFONT hFont = CreateFont(72, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _T("Courier New"));
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	TextOut(hdc, x, y, text, _tcslen(text));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawStrPlayerName(HDC hdc, int playerID, TCHAR* playerName) {
	int x, y;
	TCHAR frogImg[256] = TEXT("Frog1.bmp");
	if (playerID == 1) {
		x = 10;
		y = 500;
		_tcscpy_s(frogImg, 256, TEXT("Frog1.bmp"));
	}
	else if (playerID == 2) {
		x = 10;
		y = 550;
		_tcscpy_s(frogImg, 256, TEXT("Opponent1.bmp"));
	}

	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, frogImg, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
			BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap and clean up resources
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
	}

	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkColor(hdc, RGB(0, 0, 0)); // Set background color to black

	HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _T("Courier New"));
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	TextOut(hdc, x+35, y+7, playerName, _tcslen(playerName));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void DrawStrScore(HDC hdc, int playerID, TCHAR* score) {
	int x, y;
	if (playerID == 1) {
		x = 200;
		y = 500;
	}
	else if (playerID == 2) {
		x = 200;
		y = 550;
	}
	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkColor(hdc, RGB(0, 0, 0)); // Set background color to black

	HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, _T("Courier New"));
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	TextOut(hdc, x, y, score, _tcslen(score));

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DA JANELA
// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da estrutura "wcApp", feita no início de // WinMain(), se identifique essa função. Neste caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pré-processadas no loop "while" da função WinMain()
// Parâmetros:
//		hWnd	O handler da janela, obtido no CreateWindow()
//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//		wParam	O parâmetro wParam da estrutura messg (a mensagem)
//		lParam	O parâmetro lParam desta mesma estrutura
//
// NOTA:Estes parâmetros estão aqui acessíveis o que simplifica o acesso aos seus valores
//
// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem recebida e a tratar.
// Estas mensagens são identificadas por constantes (p.e. WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	int nRoads = 10;//number of roads

	switch (messg) {
	case WM_CLOSE: //Clicar para fechar a janela na cri
	case WM_DESTROY: // Destruir a janela e terminar o programa 
		// "PostQuitMessage(Exit Status)"
		if(MessageBox(hWnd, TEXT("Deseja fechar a jantela?"), TEXT("Confirmação"), MB_YESNO | MB_ICONQUESTION) == IDYES){ // Display a message box to confirm closing the window
			PostQuitMessage(0);
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Call the function to draw the roads
		DrawRoads(hdc, nRoads); // Change the number of roads here

		DrawFrog(hdc, nRoads); //DrawFrogs

		DrawCar(hdc, nRoads, 1,5); // Draw car
		DrawCar(hdc, nRoads, 2, 3); // Draw car
		DrawCar(hdc, nRoads, 3, 14); // Draw car
		DrawCar(hdc, nRoads, 3, 15); // Draw car
		DrawCar(hdc, nRoads, 3, 16); // Draw car
		DrawCar(hdc, nRoads, 4, 10); // Draw car
		DrawCar(hdc, nRoads, 5, 6); // Draw car
		DrawCar(hdc, nRoads, 6, 2); // Draw car
		DrawCar(hdc, nRoads, 6, 3); // Draw car
		DrawCar(hdc, nRoads, 7, 17); // Draw car
		DrawCar(hdc, nRoads, 8, 13); // Draw car

		DrawObstacle(hdc, nRoads, 3, 13);
		DrawObstacle(hdc, nRoads, 6, 4);

		DrawOpponent(hdc, nRoads, 6, 7);

		DrawStrTitle(hdc);
		
		TCHAR level[] = TEXT("Level 1");
		DrawStrLevel(hdc, level);

		TCHAR playerName[] = TEXT("Player1");
		DrawStrPlayerName(hdc, 1, playerName);
		TCHAR opponentName[] = TEXT("Player2");
		DrawStrPlayerName(hdc, 2, opponentName);


		TCHAR score[] = TEXT("Score: 50");
		DrawStrScore(hdc, 1, score);
		DrawStrScore(hdc, 2, score);

		TCHAR time[] = TEXT("00:30");
		DrawStrTime(hdc, time);
		

		EndPaint(hWnd, &ps);
		break;
	}
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") não é efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break; // break tecnicamente desnecessário por causa do return
	}
	return(0);
}