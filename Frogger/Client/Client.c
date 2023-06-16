#include "Client.h"

/* ===================================================== */
/* Programa base (esqueleto) para aplica��es Windows     */
/* ===================================================== */
// Modelo para programas Windows:
//  Composto por 2 fun��es: 
//	WinMain()     = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recep��o de mensagens provenientes do Windows
//     TrataEventos()= Processamentos da janela (pode ter outro nome)
//			1) � chamada pelo Windows (callback) 
//			2) Executa c�digo em fun��o da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
ClientData cData;
HWND g_hWnd = NULL;
// Nome da classe da janela (para programas de uma s� janela, normalmente este nome � 
// igual ao do pr�prio programa) "szprogName" � usado mais abaixo na defini��o das 
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Frogger");

// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de processamento da janela)
// Par�metros:
//   hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa 
//   hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0 destinada a conter par�metros para o programa 
//   nCmdShow:  Par�metro que especifica o modo de exibi��o da janela (usado em ShowWindow()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; // hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX � uma estrutura cujos membros servem para definir as caracter�sticas da classe da janela
	ClientCommsData communicationData;
	GameInfo g;

	// TODO Check if the server is running
	if (!isServerRunning(&communicationData, &g))
		return -1;

	// TODO CreateThread for communication with server

// ============================================================================
// 1. Defini��o das caracter�sticas da janela "wcApp" 
//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Inst�ncia da janela actualmente exibida ("hInst" � par�metro de WinMain e vem inicializada da�)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endere�o da fun��o de processamento da janela ("TrataEventos" foi declarada no in�cio e encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW; // Estilo da janela: Fazer o redraw se for modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // "hIcon" = handler do �con normal // "NULL" = Icon definido no Windows // "IDI_APPLICATION" �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // "hIconSm" = handler do �con pequeno // "NULL" = Icon definido no Windows // "IDI_INFORMATION" �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato) // "NULL" = Forma definida no Windows // "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);  // "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  "GetStockObject".Neste caso o fundo ser� branco

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
		TEXT("Frogger"), // Texto que figura na barra do t�tulo
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), // Estilo da janela (WS_OVERLAPPED= normal) // Remove resizable borders and maximize button
		CW_USEDEFAULT, // Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT, // Posi��o y pixels (default=abaixo da �ltima)
		635, // Largura da janela (em pixels)
		635, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de outra) ou HWND_DESKTOP se a janela for a primeira, criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da inst�ncia do programa actual ("hInst" � passado num dos par�metros de WinMain()
		&g); // N�o h� par�metros adicionais para a janela

	// ============================================================================
	// 4. Mostrar a janela
	// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e. normal/modal); � passado como par�metro de WinMain()
	UpdateWindow(hWnd); // Refrescar a janela (Windows envia � janela uma mensagem para pintar, mostrar dados, (refrescar)� 

	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa fila de espera at� que GetMessage(...) possa ler "a mensagem seguinte"	
	// Par�metros de "getMessage":
	// 1)"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no in�cio de WinMain()):
	//			HWND hwnd		handler da janela a que se destina a mensagem
	//			UINT message		Identificador da mensagem
	//			WPARAM wParam		Par�metro, p.e. c�digo da tecla premida
	//			LPARAM lParam		Par�metro, p.e. se ALT tamb�m estava premida
	//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
	//			POINT pt		Localiza��o do mouse (x, y) 
	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem receber as mensagens para todas as janelas pertencentes � thread actual)
	// 3)C�digo limite inferior das mensagens que se pretendem receber
	// 4)C�digo limite superior das mensagens que se pretendem receber

	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela, terminando ent�o o loop de recep��o de mensagens, e o programa 
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);// Pr�-processamento da mensagem (p.e. obter c�digo ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que aguarda at� que a possa reenviar � fun��o de tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
		// TODO close all handles and wait for communication thread to exit
	WaitForSingleObject(communicationData.hCommunicationThread, INFINITE);
	DisconnectNamedPipe(communicationData.hServerPipe);
	CloseHandle(communicationData.hServerPipe);
	return((int)lpMsg.wParam); // Retorna sempre o par�metro wParam da estrutura lpMsg
}

// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da estrutura "wcApp", feita no in�cio de // WinMain(), se identifique essa fun��o. Neste caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr�-processadas no loop "while" da fun��o WinMain()
// Par�metros:
//		hWnd	O handler da janela, obtido no CreateWindow()
//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//		wParam	O par�metro wParam da estrutura messg (a mensagem)
//		lParam	O par�metro lParam desta mesma estrutura
//
// NOTA:Estes par�metros est�o aqui acess�veis o que simplifica o acesso aos seus valores
//
// A fun��o EndProc � sempre do tipo "switch..." com "cases" que descriminam a mensagem recebida e a tratar.
// Estas mensagens s�o identificadas por constantes (p.e. WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	GameInfo* g = NULL;
	HWND hUsernameTextbox = NULL;
	TCHAR username[10];

	if (messg != WM_CREATE)
		g = (GameInfo*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (messg) {
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		g = (GameInfo*)cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)g);

		cData.screen = MENU;
		cData.nRoads = 10;
		cData.level = 1;
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
		cData.obstacle[0][1].pos.y = 6;
		cData.time = 30;
		break;
	}
	case WM_CLOSE: //Clicar para fechar a janela na cri
	case WM_DESTROY: // Destruir a janela e terminar o programa 
		// "PostQuitMessage(Exit Status)"
		if (MessageBox(hWnd, TEXT("Do you want to exit?"), TEXT("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES) { // Display a message box to confirm closing the window
			PostQuitMessage(0);
		}
		break;
	case WM_KEYDOWN:
	{
		if (cData.screen == COMPETITIVE_GAME || cData.screen == INDIVIDUAL_GAME) {
			int key = wParam;
			// Handle the key press here
			switch (key) {
			case VK_LEFT:
				cData.frog[0].pos.x--;
				//TODO communicate
				break;
			case VK_RIGHT:
				cData.frog[0].pos.x++;
				//TODO communicate
				break;
			case VK_UP:
				cData.frog[0].pos.y--;
				//TODO communicate
				break;
			case VK_DOWN:
				cData.frog[0].pos.y++;
				//TODO communicate
				break;
				// Add cases for other keys you want to handle
			default:
				// Handle other keys
				// ...
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMPETITIVE_BUTTON:
		case IDC_INDIVIDUAL_BUTTON:
			hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			GetWindowText(hUsernameTextbox, username, sizeof(username) / sizeof(username[0]));

			// Determine which button was clicked
			if (LOWORD(wParam) == IDC_COMPETITIVE_BUTTON) {
				// Competitive button was clicked
				// Handle the logic accordingly
				cData.screen = COMPETITIVE_WAIT;
			}
			else if (LOWORD(wParam) == IDC_INDIVIDUAL_BUTTON) {
				// Individual button was clicked
				// Handle the logic accordingly
				cData.screen = INDIVIDUAL_WAIT;
			}

			// Retrieve the handles of the username textbox and buttons
			HWND hUsernameTextbox = GetDlgItem(hWnd, IDC_USERNAME_TEXTBOX);
			HWND hCompetitiveButton = GetDlgItem(hWnd, IDC_COMPETITIVE_BUTTON);
			HWND hIndividualButton = GetDlgItem(hWnd, IDC_INDIVIDUAL_BUTTON);

			// Destroy the username textbox and buttons
			DestroyWindow(hUsernameTextbox);
			DestroyWindow(hCompetitiveButton);
			DestroyWindow(hIndividualButton);

			// Now you have the entered username in the "username" variable
			// and you know which button was clicked
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		switch (cData.screen) {
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
			//TODO: BlackScreen
			break;
		}

		EndPaint(hWnd, &ps);
		break;
	}
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}

	return 0;
}