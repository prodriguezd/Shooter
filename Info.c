#include "Info.h"
#define EXT2 0xEF53

void showinfoFAT(fs_FAT fat) {
    char* buffer = (char*)malloc(sizeof(char));
    int bytes;

    write(1, START, strlen(START));

    bytes = sprintf(buffer, TIPO_FICHERO, fat.filesystem);
    write(1, buffer, bytes);

    bytes = sprintf(buffer, INFO_FAT16, fat.systemName, fat.sizeSector, fat.clusterPerSectors, fat.reservedSectors, fat.numFat, fat.maxRootEntries, fat.fatSectors, fat.label);
    write(1, buffer, bytes);
}

fs_FAT INFO_getInfoFat(int fd) {
    fs_FAT fat;

    
    lseek(fd, 3, SEEK_SET);                        //system name
    fat.systemName = (char*)malloc(sizeof(char));
    read(fd, fat.systemName, 8);

    lseek (fd, 11, SEEK_SET);                      //sector size
    read(fd, &fat.sizeSector, sizeof(unsigned short));

    lseek(fd, 13, SEEK_SET);                       //cluster
    read(fd, &fat.clusterPerSectors, sizeof(unsigned char));

    lseek(fd, 14, SEEK_SET);                       //sectores reservados
    read(fd, &fat.reservedSectors, sizeof(unsigned short));

    lseek(fd, 16, SEEK_SET);                       //numero de fat's
    read(fd, &fat.numFat, sizeof(unsigned char));

    lseek(fd, 17, SEEK_SET);                       //max root entries
    read(fd, &fat.maxRootEntries, sizeof(unsigned short));

    lseek(fd, 22, SEEK_SET);                       //sectores por fat
    read(fd, &fat.fatSectors, sizeof(unsigned short));

    lseek(fd, 43, SEEK_SET);                       //volume label
    fat.label = (char*)malloc(sizeof(char));
    read(fd, fat.label, 11);

    lseek(fd, 54, SEEK_SET);                       //filesystem type
    fat.filesystem = (char*)malloc(sizeof(char));
    read(fd, fat.filesystem, 8);

    return fat;
}

void showinfoEXT(fs_ext2 ext) {
    char* buffer = (char*)malloc(sizeof(char));
    int bytes;

    write(1, START, strlen(START));

    bytes = sprintf(buffer, TIPO_FICHERO, ext.filesystem);
    write(1, buffer, bytes);

    bytes = sprintf(buffer, INFO_INODE_EXT2, ext.inodeInfo.inodeSize, ext.inodeInfo.numInodes, ext.inodeInfo.firstInode, ext.inodeInfo.inodesGroup, ext.inodeInfo.freeInodes);
    write(1, buffer, bytes);

    bytes = sprintf(buffer, INFO_BLOCK_EXT2, ext.blockInfo.blockSize, ext.blockInfo.numReservedBlocks, ext.blockInfo.numFreeBlocks, ext.blockInfo.numBlocks, ext.blockInfo.firstBlock, ext.blockInfo.blocksGroup, ext.blockInfo.fragsGroup);
    write(1, buffer, bytes);
	
    bytes = sprintf(buffer, INFO_VOLUM_EXT2, ext.volumeInfo.volumeName, ctime(ext.volumeInfo.lastCheck), ctime(ext.volumeInfo.lastMount), ctime(ext.volumeInfo.lastWrite));
    write(1, buffer, bytes);
	
	
}

fs_ext2 INFO_getInfoExt(int fd) {

    fs_ext2 ext;
    int superblock = 1024;

    ext.filesystem = (char*)malloc(sizeof(char));
    strcpy(ext.filesystem, "EXT2");

    lseek(fd, 0 + superblock, SEEK_SET);            //num inodes
    read(fd, &ext.inodeInfo.numInodes, 4);

    lseek(fd, 16 + superblock, SEEK_SET);           //free inodes
    read(fd, &ext.inodeInfo.freeInodes, 4);

    lseek(fd, 40 + superblock, SEEK_SET);           //inodes group
    read(fd, &ext.inodeInfo.inodesGroup, 4);

    lseek(fd, 84 + superblock, SEEK_SET);           //primer inode
    read(fd, &ext.inodeInfo.firstInode, 4);

    lseek(fd, 88 + superblock, SEEK_SET);           //inode size
    read(fd, &ext.inodeInfo.inodeSize, 4);


    lseek(fd, 4 + superblock, SEEK_SET);            //numero de blocks
    read(fd, &ext.blockInfo.numBlocks, 4);


    lseek(fd, 8 + superblock, SEEK_SET);            //blocks reservados
    read(fd, &ext.blockInfo.numReservedBlocks, 4);

    lseek(fd, 12 + superblock, SEEK_SET);           //blocks libres
    read(fd, &ext.blockInfo.numFreeBlocks, 4);

    lseek(fd, 20 + superblock, SEEK_SET);           //fist block
    read(fd, &ext.blockInfo.firstBlock, 4);

    lseek(fd, 24 + superblock, SEEK_SET);           //medida block
    read(fd, &ext.blockInfo.blockSize, 4);
    ext.blockInfo.blockSize = 1024 << ext.blockInfo.blockSize;
    
    lseek(fd, 32 + superblock, SEEK_SET);           //grupo block
    read(fd, &ext.blockInfo.blocksGroup, 4);

    lseek(fd, 36 + superblock, SEEK_SET);           //frags grou
    read(fd, &ext.blockInfo.fragsGroup, 4);
	
	ext.volumeInfo.lastMount = malloc(sizeof(time_t));
    lseek(fd, 44 + superblock, SEEK_SET);           //last mount
    read(fd, ext.volumeInfo.lastMount, 4);
  
    ext.volumeInfo.lastWrite = malloc(sizeof(time_t));
    lseek(fd, 48 + superblock, SEEK_SET);           //last write
    read(fd, ext.volumeInfo.lastWrite, 4);
    
	ext.volumeInfo.lastCheck = malloc(sizeof(time_t));
    lseek(fd, 64 + superblock, SEEK_SET);           //last check
    read(fd, ext.volumeInfo.lastCheck, 4);

	ext.volumeInfo.volumeName = (char*)malloc(sizeof(char));

    lseek(fd, 120 + superblock, SEEK_SET);           //volume name
    read(fd, ext.volumeInfo.volumeName, 16);
        

    return ext;

}


int INFO_getType(int fd) {

    char* string = (char*)malloc(sizeof(char));
    int index;
    lseek(fd, 54, SEEK_SET);

    int bytes = read(fd, string, 8);
    
    for (index = 0; index < bytes; index++) {       //limpiamos la cadena
        if (string[index] == ' ') {
            string[index] = '\0';
            break;
        }
    }

    if (strcmp(string, "FAT16") == 0) return 1;     //es FAT16
    
    else {                                          //miramos si es ext2
        int superblockk = 1024;
        short aux;

        lseek(fd, 56 + superblockk, SEEK_SET);
        read(fd, &aux, 2);

        if (aux == (short)EXT2) return 2;           //es Ext2
    }

    return 0;
}
 



