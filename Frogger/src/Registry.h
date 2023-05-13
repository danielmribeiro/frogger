#ifndef _REGISTRY_H_
#define _REGISTRTY_H_

#include "Base.h"

LSTATUS createRegistryKey(TCHAR* keyPath, HANDLE* hKey);
LSTATUS openRegistryKey(TCHAR* keyPath, HANDLE* hKey);
LSTATUS readValueFromRegistryKey(HANDLE hKey, TCHAR* keyName, DWORD* keyValue);

#endif