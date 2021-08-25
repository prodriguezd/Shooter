#ifndef _FIND_H_
#define _FIND_H_

#define _GNU_SOURCE 1

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "Structs.h"

#define FILE_FOUND "\nFitxer trobat. Ocupa %d bytes."
#define DIRECTORY_FOUND "Fitxer trobat. Ocupa %d bytes.\n"
#define FILE_NOT_FOUND "Error. Fitxer no trobat.\n"
#define ENTRY "%s\n"

void FIND_fileExt2(int fd, fs_ext2 ext, char *filename);
unsigned long FIND_findFileFat(int fd, fs_FAT fat, char* filename);
int getInodeTableAddr(int inode);
Entry_ext2 readEntry(int fd, int offset);
void FIND_searchFileExt2(unsigned int fd, fs_ext2 ext, char *filename);

#endif
