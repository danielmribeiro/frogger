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

bool initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->hMutexStop = NULL;
	s->speed = -1;
	s->lanes = -1;

	if (!(s->hMutexStop = CreateMutex(NULL, FALSE, SERVER_GAME_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	if (!(s->hMutex = CreateMutex(NULL, FALSE, SERVER_MEMORY_MUTEX))) {
		_tprintf(_T("Error creating mutex. Shutting down"));
		return false;
	}

	return true;
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

bool createThread(HANDLE * h, LPTHREAD_START_ROUTINE f, LPVOID ptrData) {
	*h = CreateThread(NULL,
		0,
		f,
		ptrData,
		0,
		NULL);

	if (*h == NULL) return false;
	return true;
}

DWORD WINAPI handleGame(LPVOID p) {
	int typeOfGame = lookForClients();

	ServerData* s = (ServerData*)p;
	handleNewGame(s, typeOfGame);

	//	// Generate game data
	//	setGameData(&(s->g), 0, s->speed, s->lanes);

	//// Game loop
	//while (!s->status && WaitForSingleObject(s->hMutexStop, INFINITE)) {
	//	// Move cars

	//}
}


int getRandomValue(int max) {
	srand(time(NULL));
	return rand() % (max + 1);
}

void handleCommands(ServerData* data) {
	TCHAR cmd[128];
	while (1) {
		_fgetts(cmd, 128, stdin);
		if (_tcsicmp(COMMAND_DEMO, cmd) == 0) {
			handleNewGame(data, GAME_DEMO);
		}
		else if (_tcsicmp(COMMAND_SUSPEND, cmd) == 0) {
			//TO DO SUSPEND GAME
		}
		else if (_tcsicmp(COMMAND_RESUME, cmd) == 0) {
			//TO DO RESUME GAME
		}
		else if (_tcsicmp(COMMAND_RESTART, cmd) == 0) {
			//TO DO RESTART GAME
		}
		else if (_tcsicmp(COMMAND_EXIT, cmd) == 0) {
			//TO DO EXIT GAME
		}
		else if (_tcsicmp(COMMAND_QUIT, cmd) == 0) {
			
			break;//temporary

			//send warning to other apps
			//close_serverapp(ERROR_SUCCESS_COMMAND_QUIT, data);
		}
		else {
			_tprintf(_T("Command not found!"));
		}
	}
}

int lookForClients() {
	//while(true){
		//WAIT FOR CLIENTS
		//SAVE CLIENTS INFO
		//CHECK IF CLIENT IS READY THEN BREAK
	//}

	return 0; //DEMO
	//INDIVIDUAL - return 1;
	//COMPETITIVE -	return 2;
}

bool isPositionEmpty(GameInfo* g, int x, int y) {
	// Cars
	for (int i = 0; i < MAX_LANES; i++)
		for (int j = 0; j < MAX_CARS; j++)
			if (g->cars[i][j].pos.x == x &&
				g->cars[i][j].pos.y == y)
				return false;

	// TODO Obstacules

	return true;
}

// TODO add difficulty increase due to changing level
void setGameData(GameInfo* g, int level, int speed, int lanes) {
	g->level = level;
	g->lanes = lanes;
	g->speed = speed;

	// Init cars
	for (int i = 0; i < 10; i++) {
		g->numCars[i] = 1; // TODO difficulty change here

		for (int j = 0, x = -1, y = -1; j < 8; j++) {
			do {
				x = getRandomValue(19);
				y = getRandomValue(19);
			} while (!isPositionEmpty(g, x, y));

			g->cars[i][j].pos.x = x;
			g->cars[i][j].pos.y = y;
		}
	}
}







void handleNewGame(ServerData* data, int typeOfGame) {
	switch (typeOfGame) {
	case GAME_DEMO:
		handleDemoGame();
		break;
	case GAME_INDIVIDUAL:
		//TODO HANDLE INDIVIDUAL GAME
		break;
	case GAME_COMPETITIVE:
		//TODO HANDLE COMPETITIVE GAME
		break;
	default:
		_tprintf(_T("Game mode not found!"));
		break;
	}
}

void handleDemoGame() {
	//TODO Force Exit Current Game
	//TODO Disconnect all the players connected
	//TODO Fill gameBoard with demo game data
	//Start Game
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
