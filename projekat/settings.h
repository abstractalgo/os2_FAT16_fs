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
#define APPLICATION_SETUP 2

#if (APPLICATION_SETUP == 1)
#   define moj_main
#elif (APPLICATION_SETUP == 2)
#   define javni_test
#    define IN_TYPE 4    // ------ INPUT -------------
#    if (IN_TYPE==1)
#        define IN_NAME "testovi\\ulaz.txt"
#        define OUT_NAME "testovi\\izlaz"
#        define OUT_EXT ".txt"
#    elif (IN_TYPE==2)
#        define IN_NAME "testovi\\kb80.jpg"
#        define OUT_NAME "testovi\\_kb80"
#        define OUT_EXT ".jpg"
#    elif (IN_TYPE==3)
#        define IN_NAME "testovi\\kb500.jpg"
#        define OUT_NAME "testovi\\_kb500"
#        define OUT_EXT ".jpg"
    #elif (IN_TYPE==4)
#        define IN_NAME "testovi\\kb2000.jpg"
#        define OUT_NAME "testovi\\_kb2000"
#        define OUT_EXT ".jpg"
#    endif
#endif