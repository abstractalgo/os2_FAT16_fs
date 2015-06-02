#include "misc.h"

using namespace misc;

void register_fopen(FileItem*& _root, KernelFile* _f)
{
    FileItem* fi = new FileItem(_f, _root);
    _root = fi;
}

void unregister_fopen(FileItem*& _root, KernelFile* _f)
{
    FileItem* temp = _root;
    FileItem* old = 0;
    while (temp)
    {
        if (temp->file == _f)
        {
            if (old)
            {
                old->next = temp->next;
            }
            else
            {
                _root = temp->next;
            }
            delete temp; // check
        }
        old = temp;
        temp = temp->next;
    }
}

void request_file_access(FileAccessItem*& _root, void* _thread)
{
    FileAccessItem* fai = new FileAccessItem(_thread, _root);
    _root = fai;
}

void release_file_access(FileAccessItem*& _root, void* _thread)
{
    FileAccessItem* temp = _root;
    FileAccessItem* old = 0;
    while (temp)
    {
        if (temp->thread == _thread)
        {
            if (old)
            {
                old->next = temp->next;
            }
            else
            {
                _root = temp->next;
            }
            delete temp; // check
        }
        old = temp;
        temp = temp->next;
    }
}