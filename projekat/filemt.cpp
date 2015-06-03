#include "filemt.h"

void filemt::register_fopen(OpenedFile*& _root, KernelFile* _f)
{
    OpenedFile* fi = new OpenedFile(_f, _root);
    _root = fi;
}

void filemt::unregister_fopen(OpenedFile*& _root, KernelFile* _f)
{
    OpenedFile* temp = _root;
    OpenedFile* old = 0;
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

void filemt::request_file_access(WaitingThread*& _root, void* _thread)
{
    WaitingThread* fai = new WaitingThread(_thread, _root);
    _root = fai;
}

void filemt::release_file_access(WaitingThread*& _root, void* _thread)
{
    WaitingThread* temp = _root;
    WaitingThread* old = 0;
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