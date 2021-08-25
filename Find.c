#include "Find.h"
#define SUPERBLOCK 1024
#define SUPERBLOCK_SIZE 1024

fs_ext2 ext;
fs_FAT fat;

void limpiaCadena(char *cadena, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (cadena[i] == ' ') {
            cadena[i] = '\0';
            
            break;
        }
    }

}


Entry_fat FatEntry(int fd, unsigned int entryPosition) {
    Entry_fat entry;

    lseek(fd, entryPosition + 11, SEEK_SET);

    read(fd, &entry.fileType, 1);

    lseek(fd, entryPosition + 26, SEEK_SET);
    read(fd, &entry.cluster, 2);
 
    lseek(fd, entryPosition + 28, SEEK_SET);
    read(fd, &entry.size, 4);


    return entry;
}

unsigned long readSubdirFat(int fd, unsigned int cluster, char* filename) {
    unsigned int entryposition;
    int i, bytes;
    unsigned long filePosition = 0;

    char* name = (char*) malloc (sizeof (char));
    char* extension = (char*) malloc (sizeof (char));

    entryposition = ((cluster - 2) * fat.clusterPerSectors) + fat.reservedSectors + (fat.numFat * fat.fatSectors);
    entryposition = entryposition + ((fat.maxRootEntries * 32) + (fat.sizeSector - 1)) / fat.sizeSector;
    entryposition = entryposition * fat.sizeSector;

    Entry_fat entry;
    for (i = 0; i < (int)fat.maxRootEntries; i ++) {
        entry = FatEntry(fd, entryposition);



        lseek(fd, entryposition, SEEK_SET);
        bytes = read(fd, name, 8);
        name[bytes] = '\0';

        if (bytes > 0) {
            limpiaCadena(name, bytes);

            bytes = read(fd, extension, 3);
            extension[bytes] = '\0';
            limpiaCadena(extension, bytes);
            

            if (name[0] != -27) {
                
                char *buffer = (char*) malloc(sizeof(char));
                

                if ((entry.fileType & 0x20) == 0x20) {
                    strcat(name, ".");
                    strcat(name, extension);

                    if (strcasecmp(filename, name) == 0) {
                        return entryposition;
                    }
                }
                else if ((entry.fileType & 0x10) == 0x10) {
                    if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0) {
                        if (entry.cluster != 0) {
                            filePosition = readSubdirFat(fd, entry.cluster, filename);
                            if (filePosition != 0) return filePosition;
                        }
                    }
                }
                
                free(buffer);
                
            }
            
        }
        entryposition = entryposition + 32;
    }
    free(name);
    free(extension);
    return filePosition;

}

unsigned long  FIND_findFileFat(int fd, fs_FAT fat16, char* filename) {
    fat = fat16;

    unsigned int rootDir, entryPosition;
    unsigned long filePosition = 0;
    int i, bytes;
    Entry_fat entry;

    char* name = (char*) malloc (sizeof (char));
    char* extension = (char*) malloc (sizeof (char));
    char *buffer = (char*) malloc(sizeof(char));

    rootDir = (fat.reservedSectors * fat.sizeSector) + (fat.numFat * fat.fatSectors * fat.sizeSector);

    

    entryPosition = rootDir;

    for (i = 0; i < (int)fat.maxRootEntries; i++) {
        
        entry = FatEntry(fd, entryPosition);

        lseek(fd, entryPosition, SEEK_SET);
        bytes = read(fd, name, 8);
        name[bytes] = '\0';

        if (bytes > 0) {
            limpiaCadena(name, bytes);

            bytes = read(fd, extension, 3);
            extension[bytes] = '\0';
            limpiaCadena(extension, bytes);
            

            if (name[0] != -27) {

                if ((entry.fileType & 0x20) == 0x20) {
                    strcat(name, ".");
                    strcat(name, extension);

                    if (strcasecmp(filename, name) == 0) {
                        filePosition = entryPosition;
                        break;
                    }
                }
                else if ((entry.fileType & 0x10) == 0x10) {
                    if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0) {
                        if (entry.cluster != 0) {
                            filePosition = readSubdirFat(fd, entry.cluster, filename);
                        }
                    }
                }
                
            }
        }
        entryPosition = entryPosition + 32;
    }

    if (filePosition != 0) {
        entry = FatEntry(fd, filePosition);
        bytes = sprintf(buffer, FILE_FOUND, entry.size);
        write(1, buffer, bytes);
    }
    else write(1, FILE_NOT_FOUND, strlen(FILE_NOT_FOUND));

    return filePosition;

}


Entry_ext2 readEntry(int fd, int offset) {
    Entry_ext2 entry;
  

    lseek(fd, offset, SEEK_SET);
  
    read(fd, &entry.inode, 4);

    read(fd, &entry.recLength, 2);
   
    read(fd, &entry.nameLength, 1);

    read(fd, &entry.fileType, 1);

    int bytes = 0;
    entry.name = (char*)malloc(sizeof(char));
    bytes = read(fd, entry.name, (int)entry.nameLength);
    entry.name[bytes] = '\0';


    return entry;
}

int getInodeTableAddr(int inode) {
    if (inode == 2) return 0;
    else return ((inode - 1) / ext.inodeInfo.inodesGroup) * ext.blockInfo.blocksGroup * ext.blockInfo.blockSize;
}

int readDirExt(int fd, char* filename, int inode) {
    int found = 0;
    unsigned int inodeTable, inodeTableAddr, position;

    inodeTableAddr = getInodeTableAddr(inode);

    if (ext.blockInfo.firstBlock == 0) {
        lseek(fd, ext.blockInfo.blockSize + inodeTableAddr + 8, SEEK_SET);
        read(fd, &inodeTable, 4);
    }
    else {
        lseek(fd, SUPERBLOCK + SUPERBLOCK_SIZE + inodeTableAddr + 8, SEEK_SET);
        read(fd, &inodeTable, 4);
    }
    position = (inode - 1)%ext.inodeInfo.inodesGroup;
    unsigned int root;

    if (inode == 2) root = inodeTable * ext.blockInfo.blockSize + ext.inodeInfo.inodeSize;
    else root = inodeTable * ext.blockInfo.blockSize + (ext.inodeInfo.inodeSize * position) + inodeTableAddr;

    unsigned int inodeSize; 
    lseek(fd, 4 + root, SEEK_SET);
    read(fd, &inodeSize, 4);

    unsigned int numEntries = inodeSize / ext.blockInfo.blockSize;
    unsigned int pointer, blockAddr, index, access;
    

    Entry_ext2 entry;

    for (index = 0; index < numEntries; index++) {
        if (inode == 2) lseek(fd, (40 + (index * 4) + root), SEEK_SET);
        else lseek(fd, (40 + (index * ext.blockInfo.blockSize) + root), SEEK_SET);

        read(fd, &pointer, 4);

        if (pointer != 0) {
            blockAddr = pointer * ext.blockInfo.blockSize;
            int access = 0;

            while (access < inodeSize) {
                
                entry = readEntry(fd, blockAddr);

                if (entry.fileType & 0x02) {
                    if (strcmp(entry.name, ".") != 0 && strcmp(entry.name, "..") != 0) {
                        found = readDirExt(fd, filename, entry.inode);
                        if (found != 0) return found;
                    }
                }
                
                if (entry.fileType & 0x01) {
                    if (strcmp(entry.name, filename) == 0) {
                        return (inodeTable * ext.blockInfo.blockSize) + (entry.inode -1) * ext.inodeInfo.inodeSize;
                    }
                }

                blockAddr += entry.recLength;

                access += entry.recLength;
            }
        }

    }
    return found; 
}

void FIND_fileExt2(int fd, fs_ext2 ext2, char* filename) {
    ext = ext2;
    unsigned int inodeFile = 0;
    char* buffer = (char*)malloc(sizeof(char));
    inodeFile = readDirExt(fd, filename, 2);

    if (inodeFile != 0) {
        int size;
        lseek(fd, inodeFile + 4, SEEK_SET);
        read(fd, &size, 4);

        int b = sprintf(buffer, FILE_FOUND, size);
        write(1, buffer, b);        
    }
    else write(1, FILE_NOT_FOUND, strlen(FILE_NOT_FOUND));


}




