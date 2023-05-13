#include "ServerUtil.h"

bool isProgramUnique(HANDLE* hMutex, LPCSTR filename) {
	hMutex = CreateMutex(NULL, TRUE, filename);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return false;
	}

	return true;
}

void readArguments(ServerData* s, int size, TCHAR* args[]) {
	for (int i = 1; i < size; i++) {
		if (_tcsicmp(args[i], _T("-s")) == 0) {
			s->speed = _ttoi(args[i + 1]);
		}
		else if (_tcsicmp(args[i], _T("-l")) == 0) {
			s->lanes = _ttoi(args[i + 1]);
		}
	}
}

bool initMemoryDLL(HINSTANCE* h) {
	if (!(h = LoadLibrary(_T("Memory.dll"))))
		return false;

	return true;
}

void initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->speed = -1;
	s->lanes = -1;
}

bool handleRegistry(ServerData* s) {
	HANDLE hKey = NULL;
	DWORD res;

	if (openRegistryKey(KEYPATH, &hKey) != ERROR_SUCCESS)
		if (createRegistryKey(KEYPATH, &hKey) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}

	if (s->speed > 0 && s->lanes > 0) {
		if (saveValueToRegistryKey(hKey, SPEED_KEY_NAME, s->speed) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
		if (saveValueToRegistryKey(hKey, LANES_KEY_NAME, s->lanes) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	if (s->speed == -1) {
		if (readValueFromRegistryKey(hKey, SPEED_KEY_NAME, &(s->speed)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	if (s->lanes == -1) {
		if (readValueFromRegistryKey(hKey, LANES_KEY_NAME, &(s->lanes)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	RegCloseKey(hKey);
	return true;
}

bool createThread(HANDLE* h, DWORD WINAPI f, LPVOID ptrData) {
	h = CreateThread(NULL,
		0,
		f,
		ptrData,
		0,
		NULL);

	if (!h) return false;
	return true;
}

void generalFroggerThreadFunction(ServerData* s) {
	//TODO start clients listener and wait for Single Object [clientsConnected = function Result] This ends when the server gets a connection that asks the game to start
	int clientsConnected = 0; //Temporary Meta 1
	//TODO create gameboard(clientsConnected, typeofgame (because 2 connected = 1comp || 2 connected = 1indiv + 1waiting))
	
	HANDLE hFroggerThread[/*clientsConnected a participar + 1*/1];

	if (createThread(hFroggerThread[0], handleOperatorCommands, NULL/*parameter*/)) {
		_tprintf(_T("Error creating operator handling thread"));
		return -5;
	}

	/* If... clients connected & participate in the game
	
	if (createThread(hFroggerThread[1], f, p)) {
		_tprintf(_T("Error creating client1 handling thread"));
		return -5;
	}

	if (createThread(hFroggerThread[2], f, p)) {
		_tprintf(_T("Error creating client2 handling thread"));
		return -5;
	}
	*/

	WaitForMultipleObjects(/*clientsConnected a participar +1*/1, hFroggerThread, TRUE, INFINITE);
	
}

// TODO add difficulty increase due to changing level
void setGameData(GameInfo* g, int level, int speed, int lanes) {
	g->level = level;
	g->lanes = lanes;
	g->speed = speed;

	// Init cars
	for (int i = 0; i < 10; i++) {
		g->numCars[i] = 0;

		for (int j = 0; j < 8; j++) {
			g->cars[i][j].pos.x = -1;
			g->cars[i][j].pos.y = -1;
		}
	}
		
	// 
}

void handleOperatorCommands() {

}

DWORD WINAPI handleCommunication(LPVOID p) {
	ServerData* s = (ServerData*)p;

	// Generate game data
	
}


//INICEI O SERVIDOR
//TRATEI TUDO DIREITINHO

//-----THREAD DO MAIN-----WHILE TRUE
// COMANDOS

//-----THREAD #1 -----------WHILE TRUE
//ESTOU À ESPERA DE CLIENTES
//RECEBI ORDEM PARA COMEÇAR O JOGO
//VOU PARAR DE ESPERAR CLIENTES PARA SE JUNTAREM AO JOGO
// ................. O QUE É QUE PRECISO DURANTE O JOGO? RECEBER ORDENS DOS CLIENTES, RECEBER ORDENS DOS OPERADORES E ENVIAR O JOGO A CADA JOGADA OU MOVIMENTAÇAO DE CARRO AO OPERADOR E AO CLIENTE...............
// VOU CRIAR O GAMEBOARD SEGUNDO O NUMERO DE CLIENTES QUE TENHO CONECTADO

							//-----#1 -> SUB-THREAD #1--------WHILE JOGO DECORRE
							//ESTOU À ESPERA DE COMANDOS DE OPERADORES
							// -> ATUALIZA MAPA()

							// -----#1 -> SUB-THREAD #2--------WHILE JOGO DECORRE
							// ESPERO COMANDOS DO CLIENTE 1
							// -> ATUALIZA MAPA()

							// -----#1 -> SUB-THREAD #3--------WHILE JOGO DECORRE
							// ESPERO COMANDOS DO CLIENTE 2
							// -> ATUALIZA MAPA()


// WAIT FOR #1 -> SUB-THREAD #1 / SUB-THREAD #2 /SUB-THREAD #3



// -----> ATUALIZA MAPA()-------
// SEMPRE QUE UM CARRO SE MOVE OU HÁ UMA JOGADA {
//	VOU ENVIAR O MEU GAMEBOARD PARA O CLIENTE
//  VOU ENVIAR O MEU GAMEBOARD PARA O OPERADOR
//}
