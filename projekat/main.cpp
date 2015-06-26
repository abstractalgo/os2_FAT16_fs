#include "settings.h"

#ifdef moj_main

#include <stdio.h>
#include "fs.h"
#include "part.h"
#include "kernel_fs.h"

int main(int argc, char** argv)
{
	Partition *p1 = new Partition("p1.ini");
	FS::mount(p1);
	FS::format('A');

	FILE *fin = fopen("ulaz.dat", "rb");
	char *buf1 = new char[32 * 1024 * 1024];
	size_t size = fread(buf1, 1, 32 * 1024 * 1024, fin);
	fclose(fin);

	File *f = FS::open("A:\\fajl1.dat", 'w');
	f->write(2047, buf1);
	f->write(2048, buf1 + 2048 - 1);
	f->write(2049, buf1 + 2 * 2048 - 1);
	f->write(2048, buf1 + 3 * 2048);
	f->write(size - 4 * 2048, buf1 + 4 * 2048);
	
	f->seek(0);

	char *buf2 = new char[32 * 1024 * 1024];
	f->read(size, buf2);

//	for (size_t i = 0; i < size; i++)
//		f->read(1, buf2 + i);

	int res1 = memcmp(buf1, buf2, size);
	int res2 = memcmp(buf1, buf2, f->getFileSize());
	int res3 = memcmp(buf1, buf2, 2047);
	int res4 = memcmp(buf1, buf2, 4095);
	int res5 = memcmp(buf1, buf2, 6144);
	int res6 = memcmp(buf1, buf2, 6146);

	delete f;

	delete[] buf1;
	delete[] buf2;

	FS::unmount('A');
	delete p1;
}

#endif