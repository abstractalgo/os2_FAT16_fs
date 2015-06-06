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

void filemt::request_file_access(AccessSem*& _root){
    AccessSem* fai = new AccessSem(CreateSemaphore(0,1,1,0), _root);
    _root = fai;
    wait(fai->sem);
}

void filemt::release_file_access(AccessSem*& _root)
{
    AccessSem* temp = _root;
    AccessSem* old = 0;

    if (!temp)
        return;

    while (temp->next)
    {
        
        old = temp;
        temp = temp->next;
    }
    signal(temp->sem);
    if (old)
    {
        old->next = 0;
    }
    delete temp;
}