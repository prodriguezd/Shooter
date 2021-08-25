#ifndef _STRUCTS_H_
#define _STRUCTS_H_
#include <time.h>

typedef struct {
    char* filesystem;
    char* systemName;
    unsigned short sizeSector;
    unsigned char clusterPerSectors;
    unsigned short reservedSectors;
    unsigned char numFat;
    unsigned short maxRootEntries;
    unsigned short fatSectors;
    char* label;

}fs_FAT;

typedef struct {
    unsigned int inodeSize;
    unsigned int numInodes;
    unsigned int firstInode;
    unsigned int inodesGroup;
    unsigned int freeInodes;
}InodesInfo;

typedef struct {
    unsigned int blockSize;
    unsigned int numReservedBlocks;
    unsigned int numFreeBlocks;
    unsigned int numBlocks;
    unsigned int firstBlock;
    unsigned int blocksGroup;
    unsigned int fragsGroup;
}BlockInfo;

typedef struct {
    char* volumeName;
    time_t *lastCheck;
    time_t *lastMount;
    time_t *lastWrite;
}VolumeInfo;

typedef struct {
    char* filesystem;
    InodesInfo inodeInfo;
    BlockInfo blockInfo;
    VolumeInfo volumeInfo;
}fs_ext2;


typedef struct {
    unsigned int inode;
    unsigned short recLength;
    char nameLength;
    char fileType;
    char* name;
}Entry_ext2;

typedef struct {
    char *name;
    char *extension;
    unsigned char fileType;
    unsigned short cluster;
    unsigned int size;
}Entry_fat;

#endif
