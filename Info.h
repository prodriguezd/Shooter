#ifndef _INFO_H_
#define _INFO_H_


#define START 			    "\n ------ Filesystem Information ------\n"
#define TIPO_FICHERO        "\nFilesystem: %s\n"
#define INFO_INODE_EXT2     "INFO INODE\nMida Inode: %d\nNumInode: %d\nPrimer Inode: %d\nInodes Grup: %d\nInodes Lliures: %d\n\n"  
#define INFO_BLOCK_EXT2     "INFO BLOC\nMida Bloc: %d\nBlocs Reservats: %d\nBlocs Lliures: %d\nTotal Blocs: %d\nPrimer Bloc: %d\nBlocs grup: %d\nFrags grup: %d\n\n"
#define INFO_VOLUM_EXT2     "INFO VOLUM\nNom Volum: %s\nUltima comprov: %sUltim muntatge: %sUltima escriptura: %s\n"
#define INFO_FAT16          "System Name: %s\nMida del sector: %d\nSectors Per Cluster: %d\nSectors reservats: %d\nNumero de FATs: %d\nMaxRootEntries: %d\nSectors per FAT: %d\nLabel: %s\n"


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "Structs.h"

int INFO_getType(int fd);
fs_FAT INFO_getInfoFat(int fd);
fs_ext2 INFO_getInfoExt(int fd);
void showinfoEXT(fs_ext2 ext);
void showinfoFAT(fs_FAT fat);


#endif