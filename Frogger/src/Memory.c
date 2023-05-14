#include "Memory.h"

SharedMemoryHandle createSharedMemory(const char* name, size_t size)
{
	SharedMemoryHandle handle = NULL;

	HANDLE fileHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		(DWORD)size,
		name);
	if (fileHandle != NULL)
	{
		handle = (SharedMemoryHandle)fileHandle;
	}

	return handle;
}

SharedMemoryHandle openSharedMemory(const char* name)
{
	SharedMemoryHandle handle = NULL;

	HANDLE fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,
		FALSE,
		name);
	if (fileHandle != NULL)
	{
		handle = (SharedMemoryHandle)fileHandle;
	}

	return handle;
}

LPVOID getMapViewOfFile(SharedMemoryHandle handle, size_t size) {
	LPVOID buffer = MapViewOfFile(handle,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		size);

	if (!buffer) return NULL;
	return buffer;
}

bool writeSharedMemory(SharedMemoryHandle handle, const void* data, size_t size)
{
	bool success = false;

	if (handle != NULL)
	{
		LPVOID buffer = MapViewOfFile(handle,
			FILE_MAP_WRITE,
			0,
			0,
			size);
		if (buffer != NULL)
		{
			memcpy(buffer, data, size);
			success = true;

			UnmapViewOfFile(buffer);
		}
	}

	return success;
}

bool readSharedMemory(SharedMemoryHandle handle, void* data, size_t size)
{
	bool success = false;

	if (handle != NULL)
	{
		LPVOID buffer = MapViewOfFile(handle,
			FILE_MAP_READ,
			0,
			0,
			size);
		if (buffer != NULL)
		{
			memcpy(data, buffer, size);
			success = true;

			UnmapViewOfFile(buffer);
		}
	}

	return success;
}

bool closeSharedMemory(SharedMemoryHandle handle, LPVOID buf)
{
	bool success = false;

	if (handle != NULL)
	{
		if (buf) UnmapViewOfFile(buf);
		success = CloseHandle((HANDLE)handle);
	}

	return success;
}
