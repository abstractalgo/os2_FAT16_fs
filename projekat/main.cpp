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

#define TREEDIR(c) tree(*FS::myImpl->disks[c-65].disk)

int main(int argc, char** argv)
{

#define testapp
#ifdef testapp

    Partition *p1 = new Partition("p1.ini");
    Partition *p2 = new Partition("p2.ini");

    char disk1, disk2;

    MARK("Mount 1");
    disk1 = FS::mount(p1);

    MARK("Format 1");
    FS::format(disk1);

    TREEDIR(disk1);

    MARK("Mount 2");
    disk2 = FS::mount(p2);

    MARK("Format 2");
    FS::format(disk2);

    printf("Da li postoji putanja A:\\? %s.\n", FS::doesExist("A:\\") ? "Da" : "Ne");
    printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");
    FS::createDir("A:\\dada");
    TREEDIR(disk1);
    printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");
    //printf("Da li postoji putanja B:\\? %s.\n", FS::doesExist("B:\\") ? "Da" : "Ne");
    //printf("Da li postoji putanja B:\\dada\\hehe? %s.\n", FS::doesExist("B:\\dada\\hehe") ? "Da" : "Ne");

    MARK("Unmount 1");
    FS::unmount(disk1);
    MARK("Unmount 2");
    FS::unmount(disk2);

    delete p1;
    delete p2;
#endif

//#define testpathparser
#ifdef testpathparser

    PathParser p;
    parse(p, p_a_t_h);
    printf("** %s\n", isValid(p)?"validno":"nevalidno");
    printf("* %s\n", combine(p, 3));
#endif

	getchar();
	return 0;
}