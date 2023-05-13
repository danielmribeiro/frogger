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

void initServerData(ServerData* s) {
	s->hMutex = NULL;
	s->speed = -1;
	s->lanes = -1;
}

LSTATUS createRegistryKey(TCHAR * keyPath, HANDLE* hKey) {
	DWORD res;
	return RegCreateKeyEx(HKEY_CURRENT_USER,
		keyPath,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		hKey,
		&res);
}

LSTATUS openRegistryKey(TCHAR * keyPath, HANDLE* hKey) {
	return RegOpenKeyEx(HKEY_CURRENT_USER,
		keyPath,
		0,
		KEY_ALL_ACCESS,
		hKey);
}

LSTATUS readValueFromRegistryKey(HANDLE hKey, TCHAR * keyName, DWORD * keyValue) {
	DWORD dwSize;

	return RegQueryValueEx(hKey, 
		keyName,
		NULL, 
		NULL, 
		(LPBYTE)keyValue, 
		&dwSize);
}

LSTATUS saveValueToRegistryKey(HANDLE hKey, TCHAR * keyName, DWORD keyValue) {
	return RegSetValueEx(hKey,
		keyName,
		0,
		REG_DWORD,
		(LPBYTE)&keyValue,
		sizeof(keyValue));
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

	if(s->speed == -1) {
		if (readValueFromRegistryKey(hKey, SPEED_KEY_NAME, &(s->speed)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	if(s->lanes == -1) {
		if (readValueFromRegistryKey(hKey, LANES_KEY_NAME, &(s->lanes)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}
	}

	RegCloseKey(hKey);
	return true;
}