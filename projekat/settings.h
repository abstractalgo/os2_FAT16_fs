#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "fs.h"
#include "part.h"
#include "kernel_fs.h"

#define MARK(t) printf("--- "t" ---\n")
#define BLBR(b) printf("\n--------------------------------------------------------- ["b"]\n")
#define CBREAK printf("\n----------------------------------------------\n")
#define TREEDIR(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk, false);CBREAK
#define TREEDIRI(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk);CBREAK
#define FATty(c) printf("FAT:\n");for(uint8_t i=0; i<10; i++){printf("[%d]: %d\n", i, FS::myImpl->disks[c-65].disk->FAT[i]);}putchar('\n');

// 1 - proba
// 2 - javni test
// 3 - moj test
#define APPLICATION_SETUP 1

#if (APPLICATION_SETUP == 1)
#   define moj_main
#elif (APPLICATION_SETUP == 2)
#   define javni_test
#elif (APPLICATION_SETUP == 3)
#   define moj_test
#endif