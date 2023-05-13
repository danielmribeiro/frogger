#include "FroggerDLL.h"

EXPORT SharedMemoryHandle CreateSharedMemory(const char* name, size_t size)
{
    SharedMemoryHandle handle = NULL;

    HANDLE fileHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)size, name);
    if (fileHandle != NULL)
    {
        handle = (SharedMemoryHandle)fileHandle;
    }

    return handle;
}

EXPORT SharedMemoryHandle OpenSharedMemory(const char* name)
{
    SharedMemoryHandle handle = NULL;

    HANDLE fileHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
    if (fileHandle != NULL)
    {
        handle = (SharedMemoryHandle)fileHandle;
    }

    return handle;
}

EXPORT bool WriteSharedMemory(SharedMemoryHandle handle, const void* data, size_t size)
{
    bool success = false;

    if (handle != NULL)
    {
        LPVOID buffer = MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, size);
        if (buffer != NULL)
        {
            memcpy(buffer, data, size);
            success = true;

            UnmapViewOfFile(buffer);
        }
    }

    return success;
}

EXPORT bool ReadSharedMemory(SharedMemoryHandle handle, void* data, size_t size)
{
    bool success = false;

    if (handle != NULL)
    {
        LPVOID buffer = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, size);
        if (buffer != NULL)
        {
            memcpy(data, buffer, size);
            success = true;

            UnmapViewOfFile(buffer);
        }
    }

    return success;
}

EXPORT bool CloseSharedMemory(SharedMemoryHandle handle)
{
    bool success = false;

    if (handle != NULL)
    {
        success = CloseHandle((HANDLE)handle);
    }

    return success;
}
