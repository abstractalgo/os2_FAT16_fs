#include "part.h"
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include "fs.h"
#include "kernel_fs.h"
#include "pathparser.h"
#include "cache.h"

#define MARK(t) printf("--- "t" ---\n")

#define p_a_t_h "X:\\huehue\\hue\\3\\cetiri.pet"

int main(int argc, char** argv)
{

#define testapp
#ifdef testapp

    Partition *p1 = new Partition("p1.ini");
    Partition *p2 = new Partition("p2.ini");

    char disk1, disk2;

    /*MARK("Mount 1");
    disk1 = FS::mount(p1);

    MARK("Format 1");
    FS::format(disk1);*/

    //MARK("Mount 2");
    //disk2 = FS::mount(p2);

    /*MARK("Format 2");
    FS::format(disk2);

    MARK("Creating dir");
    printf("Da li postoji putanja A:\\? %s.\n", FS::doesExist("A:\\") ? "Da" : "Ne");
    FS::createDir("A:\\\\dada");
    printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");*/

    /*MARK("Unmount 1");
    FS::unmount(disk1);*/
    /*MARK("Unmount 2");
    FS::unmount(disk2);*/

    delete p1;
    delete p2;
#endif

//#define testpathparser
#ifdef testpathparser

    PathParser p;
    parse(p, p_a_t_h);
    printf("** %s\n", isValid(p)?"validno":"nevalidno");
#endif

//#define testcache
#ifdef testcache
    CacheLRU c(CACHE_SIZE);
    debug_write(c);

    writeCache(c, 4, "1");
    debug_write(c);

    writeCache(c, 5, "2");
    debug_write(c);

    writeCache(c, 4, "3");
    debug_write(c);

    readCache(c, 5);
    debug_write(c);

    writeCache(c, 6, "4");
    debug_write(c);

    readCache(c, 5);
    debug_write(c);

    readCache(c, 4);
    debug_write(c);

    writeCache(c, 7, "5");
    debug_write(c);

    writeCache(c, 8, "6");
    debug_write(c);

    writeCache(c, 9, "7");
    debug_write(c);

    readCache(c, 8);
    debug_write(c);
#endif

	getchar();
	return 0;
}