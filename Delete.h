#ifndef _DELETE_H_
#define _DELETE_H_

#include "Structs.h"
#include "Find.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define DEL_FILE "El fitxer %s ha estat eliminat.\n"
#define FILE_NOT_FOUND "Error. Fitxer no trobat.\n"

void DELETE_fileExt2(int fd, fs_ext2 ext, char* filename);
void DELETE_fileFAT(int fd, fs_FAT ext, char* filename);
#endif