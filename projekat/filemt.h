#pragma once

#include "semaphore.h"

class KernelFile;

namespace filemt
{
    // ------------------ struct FileItem --------------------------
    struct OpenedFile
    {
        KernelFile* file;
        OpenedFile* next;
        OpenedFile(KernelFile* _kf = 0, OpenedFile* _n = 0)
            : file(_kf)
            , next(_n)
        {}
    };

    void register_fopen(OpenedFile*& _root, KernelFile* _f);
    void unregister_fopen(OpenedFile*& _root, KernelFile* _f);

    // ------------------ struct FileAccessItem --------------------

    struct AccessSem
    {
        Semaphore sem;
        AccessSem* next;
        AccessSem(void* _sem = 0, AccessSem* _next = 0)
            : sem(_sem)
            , next(_next)
        {}
        ~AccessSem()
        {
            CloseHandle(sem);
        }
    };

    void request_file_access(AccessSem*& _root);
    void release_file_access(AccessSem*& _root);
}

typedef filemt::OpenedFile* OpenedFilesTable;