#include "part.h"
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include "fs.h"
#include "kernel_fs.h"

#define _(t) printf(t"\n");

int main(int argc, char** argv)
{
    Partition* p1 = new Partition("p1.ini");
    Partition* p2 = new Partition("p2.ini");
	char disk1, disk2;

    _("Mount 1")    disk1 = FS::mount(p1);
    _("Format 1")   FS::format(disk1);
    _("Mount 2")    disk2 = FS::mount(p2);
	_("Format 2")   FS::format(disk2);

	printf("Da li postoji putanja A:\\? %s.\n", FS::doesExist("A:\\") ? "Da" : "Ne");
	FS::createDir("A:\\\\dada");
	printf("Da li postoji putanja A:\\dada? %s.\n", FS::doesExist("A:\\dada") ? "Da" : "Ne");

    _("Unmount 1")  FS::unmount(disk1);
    _("Unmount 2")  FS::unmount(disk2);

	delete p1;
	delete p2;

#define p_a_t_h "X:\\huehue\\hue\\3\\cetiri.pet"

	PathIterator* p = new PathIterator((char*)p_a_t_h);
	printf("Putanja "p_a_t_h" je %s.\n", p->isValid() ? "validna" : "nevalidna");
	printf("%c:", p->disk());
	char* ime = 0;
	while (ime = p->next())
		p->isLast() ? printf("^%s-", ime) : printf("%s-", ime);
	putchar('\n');

	ime = p->getParent();
	printf("%s\n", ime);

	if (ime)delete[] ime;
	delete p;

	printf("%d", -1 / 8);
	
	getchar();
	return 0;
}