#pragma once

#include <windows.h>

typedef HANDLE Semaphore;
#define signal(x) ReleaseSemaphore(x,1,NULL)
#define wait(x) WaitForSingleObject(x,INFINITE)