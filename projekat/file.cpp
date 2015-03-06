#include "fs.h"
#include "file.h"
#include "kernel_file.h"

char File::write(BytesCnt cnt, char* buffer)
{
    return myImpl->write(cnt, buffer);
}

BytesCnt File::read(BytesCnt cnt, char* buffer)
{
    return myImpl->read(cnt, buffer);
}

char File::seek(BytesCnt cnt)
{
    return myImpl->seek(cnt);
}

BytesCnt File::filePos()
{
    return myImpl->filePos();
}

char File::eof()
{
    return myImpl->eof();
}

BytesCnt File::getFileSize()
{
    return myImpl->getFileSize();
}

char File::truncate()
{
    return myImpl->truncate();
}

File::~File()
{
    delete myImpl;
}

// private

File::File()
{
    myImpl = 0;
}