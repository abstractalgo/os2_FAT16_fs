#include "fs.h"
#include "kernel_fs.h"

/* ----------------------- FILE SYSTEM ----------------------- */

KernelFS* FS::myImpl = new KernelFS();

FS::FS()
{
	// TODO
}

FS::~FS()
{
    delete myImpl;
}
/* ----------------------- PARTITION ----------------------- */
char FS::mount(Partition* partition)
{
	return myImpl->mount(partition);
}

char FS::unmount(char part)
{
	return myImpl->unmount(part);
}

char FS::format(char part)
{
	return myImpl->format(part);
}
/* ----------------------- FILE ----------------------- */
char FS::doesExist(char* fname)
{
	return myImpl->doesExist(fname);
}

File* FS::open(char* fname, char mode)
{
	return myImpl->open(fname, mode);
}

char FS::deleteFile(char* fname)
{
	return myImpl->deleteFile(fname);
}
/* ----------------------- DIRECTORY ----------------------- */
char FS::createDir(char* dirname)
{
	return myImpl->createDir(dirname);
}

char FS::deleteDir(char* dirname)
{
	return myImpl->deleteDir(dirname);
}

char FS::readDir(char* dirname, EntryNum n, Entry &e)
{
	return myImpl->readDir(dirname, n, e);
}

