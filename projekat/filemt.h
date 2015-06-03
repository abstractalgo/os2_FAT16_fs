#pragma once

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

    struct WaitingThread
    {
        void* thread;
        WaitingThread* next;
        WaitingThread(void* _thread = 0, WaitingThread* _next = 0)
            : thread(_thread)
            , next(_next)
        {}
    };

    void request_file_access(WaitingThread*& _root, void* _thread);
    void release_file_access(WaitingThread*& _root, void* _thread);
}