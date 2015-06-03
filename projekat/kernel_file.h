#include "misc.h"
#include "pathparser.h"
#include "part.h"
#include "stdio.h"
#include <windows.h>
#include "fs.h"
#include "newdisk.h"

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
    PathParser ppath;
    Disk& d;

    friend class FS;
    friend class KernelFS;
    KernelFile(Disk& _d);
};