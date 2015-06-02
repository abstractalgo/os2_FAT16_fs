#include "misc.h"

class KernelFile
{
public:
    char write(BytesCnt, char* buffer);
    BytesCnt read(BytesCnt, char* buffer);
    char seek(BytesCnt);

    BytesCnt filePos();
    char eof();
    BytesCnt getFileSize();
    char truncate();
    ~KernelFile();
private:

    Entry entry;
    unsigned long caret;
    char mod;
    misc::FileAccessItem* threadtable;

    // lista pokazivaca na FIFO niti
    // trenutni modalitet


    friend class FS;
    friend class KernelFS;
    KernelFile();
};