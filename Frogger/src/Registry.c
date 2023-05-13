#include "Registry.h"

LSTATUS createRegistryKey(TCHAR* keyPath, HANDLE* hKey) {
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

LSTATUS openRegistryKey(TCHAR* keyPath, HANDLE* hKey) {
	return RegOpenKeyEx(HKEY_CURRENT_USER,
		keyPath,
		0,
		KEY_ALL_ACCESS,
		hKey);
}

LSTATUS readValueFromRegistryKey(HANDLE hKey, TCHAR* keyName, DWORD* keyValue) {
	DWORD dwSize;

	return RegQueryValueEx(hKey,
		keyName,
		NULL,
		NULL,
		(LPBYTE)keyValue,
		&dwSize);
}

LSTATUS saveValueToRegistryKey(HANDLE hKey, TCHAR* keyName, DWORD keyValue) {
	return RegSetValueEx(hKey,
		keyName,
		0,
		REG_DWORD,
		(LPBYTE)&keyValue,
		sizeof(keyValue));
}