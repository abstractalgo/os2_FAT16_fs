#pragma once

#include <windows.h>

#define Semaphore HANDLE
#define signal(x) ReleaseSemaphore(x,1,NULL)
#define wait(x) WaitForSingleObject(x,INFINITE)