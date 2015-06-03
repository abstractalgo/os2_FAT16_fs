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

#define CBREAK printf("----------------------------------------------\n")
#define TREEDIR(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk, false);CBREAK
#define TREEDIRI(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk);CBREAK

int main(int argc, char** argv)
{

#define testapp
#ifdef testapp

    Partition *p1 = new Partition("p1.ini");
    //Partition *p2 = new Partition("p2.ini");

    //MARK("Mount & format 1");
    char disk1 = FS::mount(p1);
    FS::format(disk1);

    //MARK("Mount 2");
    //char disk2 = FS::mount(p2);
    //FS::format(disk2);

    FS::createDir("A:\\grim");
    FS::createDir("A:\\grim\\vfx");
    FS::createDir("A:\\ana");
    FS::createDir("A:\\milica");
    FS::createDir("A:\\milica\\bmit");
    FS::createDir("A:\\milica\\bmit\\huehue");
    //TREEDIRI(disk1);
    FS::deleteDir("A:\\ana");
    //TREEDIRI(disk1);
    FS::createDir("A:\\AnaPetrv");
    FS::deleteDir("A:\\milica\\bmit\\huehue");
    TREEDIR(disk1);
    printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");
    printf("Da li postoji putanja A:\\grim\\vfx? %s.\n", FS::doesExist("A:\\grim\\vfx") ? "Da" : "Ne");

    //MARK("Unmount");
    FS::unmount(disk1);
    //FS::unmount(disk2);

    delete p1;
    //delete p2;
#endif

	getchar();
	return 0;
}