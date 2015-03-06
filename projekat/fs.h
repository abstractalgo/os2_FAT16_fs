#ifndef _os2_fs_
#define _os2_fs_

typedef unsigned long BytesCnt;
typedef unsigned long EntryNum;

const unsigned int FNAMELEN = 8;
const unsigned int FEXTLEN = 3;

struct Entry
{
	char name[FNAMELEN];
	char ext[FEXTLEN];
	char attributes;
	unsigned long firstCluster;
	unsigned long size;
};

class KernelFS;
class Partition;
class File;

class FS
{
public:
	~FS();

	static char mount(Partition* partition);
	static char unmount(char part);
	static char format(char part);
	static char doesExist(char* fname);
	static File* open(char* fname, char mode);
	static char deleteFile(char* fname);
	static char createDir(char* dirname);
	static char deleteDir(char* dirname);
	static char readDir(char* dirname, EntryNum n, Entry &e);

protected:
	FS();
    
    static KernelFS *myImpl;
};

#endif