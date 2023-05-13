#include "FroggerDll.h"

// Create shared memory file
HANDLE createSharedMemory(HANDLE hFile) {
    // Create shared memory
    if (!hFile) {
        hFile = INVALID_HANDLE_VALUE;
    }

    HANDLE hSharedMemory = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
    if (hSharedMemory == NULL) {
        printf("Error creating shared memory: %d\n", GetLastError());
        return NULL;
    }

    return hSharedMemory;
}

// Open shared memory file
HANDLE openSharedMemory() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_READ | FILE_MAP_WRITE,
        FALSE,
        SHARED_MEMORY_NAME);

    if (hMapFile != NULL)
    {
        pBuf = (LPTSTR)MapViewOfFile(hMapFile,
            FILE_MAP_READ,
            0,
            0,
            0);

        if (pBuf != NULL)
        {
            // Read from the shared memory here
            // ...

            UnmapViewOfFile(pBuf);
        }
        CloseHandle(hMapFile);
    }

}

// Close shared memory file
HANDLE closeSharedMemory() {}

// Read the file
// Write file
