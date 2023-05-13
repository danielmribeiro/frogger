#ifndef _FROGGERDLL_H_
#define _FROGGERDLL_H_

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#define DLL_IMP_API __declspec(dllimport)
#define SHARED_MEMORY_SIZE 2048
#define SHARED_MEMORY_NAME _T("FROGGER_SHARED_MEMORY")

DLL_IMP_API HANDLE createSharedMemory(HANDLE hFile);
DLL_IMP_API HANDLE openSharedMemory();

#endif