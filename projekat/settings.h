#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "fs.h"
#include "part.h"
#include "kernel_fs.h"

#define MARK(t) printf("--- "t" ---\n")
#define CBREAK printf("\n----------------------------------------------\n")
#define TREEDIR(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk, false);CBREAK
#define TREEDIRI(c) CBREAK;printf("%c:/",c);tree(*FS::myImpl->disks[c-65].disk);CBREAK

#define testapp