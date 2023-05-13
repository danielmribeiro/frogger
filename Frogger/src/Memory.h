#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "Base.h"

//#define EXPORT __declspec(dllexport);
#define DLL_IMP_API __declspec(dllexport)

typedef void* SharedMemoryHandle;

DLL_IMP_API SharedMemoryHandle createSharedMemory(const char* name, size_t size);
DLL_IMP_API SharedMemoryHandle openSharedMemory(const char* name);
DLL_IMP_API bool writeSharedMemory(SharedMemoryHandle handle, const void* data, size_t size);
DLL_IMP_API bool readSharedMemory(SharedMemoryHandle handle, void* data, size_t size);
DLL_IMP_API bool closeSharedMemory(SharedMemoryHandle handle);

#endif
