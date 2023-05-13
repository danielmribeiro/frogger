#ifndef _FROGGERDLL_H_
#define _FROGGERDLL_H_

#include "Base.h"

#define EXPORT __declspec(dllexport);
typedef void* SharedMemoryHandle;

EXPORT SharedMemoryHandle CreateSharedMemory(const char* name, size_t size);
EXPORT SharedMemoryHandle OpenSharedMemory(const char* name);
EXPORT bool WriteSharedMemory(SharedMemoryHandle handle, const void* data, size_t size);
EXPORT bool ReadSharedMemory(SharedMemoryHandle handle, void* data, size_t size);
EXPORT bool CloseSharedMemory(SharedMemoryHandle handle);

#endif
