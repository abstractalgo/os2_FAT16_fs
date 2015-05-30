#ifndef _os2_kfs_
#define _os2_kfs_

#include "fs.h"
#include "part.h"
#include "windows.h"
#include <stdio.h>
#include "newdisk.h"
#include <string.h>

#define test(exp) if (!(exp)) fprintf(stderr,"Assert failed: %s\nFile: %s\nLine: %d\n", (#exp),__FILE__,__LINE__)

////////////////////////////////////////////////////////////////////////////////
// Class name: Path
////////////////////////////////////////////////////////////////////////////////
class PathIterator
{
	int len;
	int idx;
	char* val;
	char* path;
public:
	PathIterator(char* p)
		: len(strlen(p))
		, idx(3)
		, path(new char[strlen(p)+1])
		, val(0)
	{
		strcpy_s(path, len + 1, p);
	}

	bool isValid()
	{
		char* dp = 0;
		int old = idx;
		bool v = true;
		while (dp = next())
		{
			if ((!isLast() && !isFolder()) || (strlen(dp) == 0))
			{
				v = false;
				break;
			}
		}
		idx = old;
		return v;
	}

	char* next()
	{
		int i = idx;

		if (idx >= len)
			return 0;

		while (path[i] != '\\' && path[i] != '\0')
			i++;

		if (val != 0) delete[] val;

		val = new char[i - idx + 1];
		val[i - idx] = '\0';
		for (int j = 0; j < i - idx; j++)
			val[j] = path[idx + j];

		idx = i+1;
		return val;
	}

	char* prev()
	{
		int i = idx - 1;
		int bckslash_cnt = 0;
		if (i < 3)
			return 0;
		while (i >= 0 && bckslash_cnt<2)
			if (path[--i] == '\\')
				bckslash_cnt++;
		if (i < 0)
			return 0;
		idx = i + 1;
		return this->next();
	}

	char* curr() const
	{
		return val;
	}

	char disk() const
	{
		return path[0];
	}

	bool isLast()
	{
		for (int i = idx-1; i < len+1; i++)
			if (path[i] == '\\')
				return false;
		return true;
	}

	char* getParent()
	{
		if (len <= 3)
			return 0;

		char* res = 0;
		int last = 3;
		for (int i = 3; i < len; i++)
		{
			if (path[i] == '\\')
				last = i + 1;
		}

		res = new char[last];
		res[last-1] = '\0';
		for (int i = 0; i < last-1; i++)
			res[i] = path[i];

		return res;
	}

	bool isFolder()
	{
		int lval = strlen(val);
		for (int i = 0; i < lval; i++)
			if (val[i] == '.')
				return false;
		return true;
	}

	char* last()
	{
		int bsidx = 0;
		for (int i = 0; i < len; i++)
		{
			if (path[i] == '\\')
				bsidx = i;
		}
		char* str = new char[len - bsidx];
		memcpy(str, path + bsidx+1, sizeof(char)*(len - bsidx));
		return str;
	}

	~PathIterator()
	{
		if (path != 0) delete[] path;
		if (val != 0) delete[] val;
	}
};

////////////////////////////////////////////////////////////////////////////////
// Class name: KernelFS
////////////////////////////////////////////////////////////////////////////////
class KernelFS
{
    struct DiskDesc
    {
        bool used;
        Disk* disk;
        DiskDesc()
            : used(false)
            , disk(0)
        {}
    };
    DiskDesc disks[26];

    KernelFS();
	~KernelFS();

	char mount(Partition* partition);
	char unmount(char part);
	char format(char part);

	char doesExist(char* fname);
	File* open(char* fname, char mode);
	char deleteFile(char* fname);

	char createDir(char* dirname);
	char deleteDir(char* dirname);
	char readDir(char* dirname, EntryNum n, Entry &e);

    Entry* getEntry(char* path);
	void deleteEntry(Disk* d, Entry* e);
	Entry* searchEntry(Disk* d, Entry* parent, char* name);
    Entry* copyEntry(Entry& e);
    bool matchName(Entry& e, char* name);
	
	friend class FS;
};

#endif