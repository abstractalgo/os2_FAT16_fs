#include "settings.h"

#ifndef testapp

#include <stdio.h>
#include "fs.h"
#include "part.h"
#include "kernel_fs.h"

int main(int argc, char** argv)
{
    Partition *p1 = new Partition("p1.ini");
    //Partition *p2 = new Partition("p2.ini");

    //MARK("Mount & format 1");sta

    char disk1 = FS::mount(p1);
    FS::format(disk1);

    //MARK("Mount 2");
    //char disk2 = FS::mount(p2);
    //FS::format(disk2);
    TREEDIR(disk1);

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

    File* f = FS::open("A:\\dragan.txt", 'w');
    char ble[4095] = "dragan je kralj";
    f->write(4095, ble);
    delete f;

    /*FS::unmount('A');
    disk1 = FS::mount(p1);*/

    f = FS::open("A:\\dragan.txt", 'r');
    char* txt = new char[16];
    f->seek(0);
    f->read(16, txt);
    printf("TXT: %.10s\n", txt);
    delete[] txt;
    delete f;

    TREEDIRI(disk1);
    printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");
    printf("Da li postoji putanja A:\\grim\\vfx? %s.\n", FS::doesExist("A:\\grim\\vfx") ? "Da" : "Ne");

    //MARK("Unmount");
    FS::unmount(disk1);
    //FS::unmount(disk2);

    delete p1;
    //delete p2;

    /*PathParser p;
    const char* path = "A:\\dragan\\okanovic\\je\\kralj.exe";
    parse(p, path);
    char* cmb;
    cmb = combine(p, 0);
    printf("'%s'\n", cmb);
    delete[] cmb;*/

    /*Partition* p1 = new Partition("p1.ini");
    char disk1 = FS::mount(p1);
    FS::format(disk1);
    TREEDIR(disk1);
    FS::unmount(disk1);
    disk1 = FS::mount(p1);
    TREEDIR(disk1);
    FS::unmount(disk1);
    delete p1;*/

	getchar();
	return 0;
}

#endif