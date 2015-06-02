#ifndef _misc_h_
#define _misc_h_

class KernelFile;

namespace misc
{
    // ------------------ struct FileItem --------------------------
    struct FileItem
    {
        KernelFile* file;
        FileItem* next;
        FileItem(KernelFile* _kf=0, FileItem* _n=0)
            : file(_kf)
            , next(_n)
        {}
    };

    void register_fopen(FileItem*& _root, KernelFile* _f);
    void unregister_fopen(FileItem*& _root, KernelFile* _f);

    // ------------------ struct FileAccessItem --------------------

    struct FileAccessItem
    {
        void* thread;
        FileAccessItem* next;
        FileAccessItem(void* _thread = 0, FileAccessItem* _next = 0)
            : thread(_thread)
            , next(_next)
        {}
    };

    void request_file_access(FileAccessItem*& _root, void* _thread);
    void release_file_access(FileAccessItem*& _root, void* _thread);
}
#endif