#include "Delete.h"
#define SUPERBLOCK 1024
#define SUPERBLOCK_SIZE 1024

void removeEntry(int fd, unsigned int blockAddr, Entry_ext2 entry) {
    char value = 0;
        lseek(fd, blockAddr, SEEK_SET);
    write(fd, &value, 1);
    lseek(fd, blockAddr + 1, SEEK_SET);
    write(fd, &value, 1);
    lseek(fd, blockAddr + 2, SEEK_SET);
    write(fd, &value, 1);
    lseek(fd, blockAddr + 3, SEEK_SET);
   int bytes =  write(fd, &value, 1);
    lseek(fd, blockAddr + 4, SEEK_SET);

    lseek(fd, blockAddr + 6, SEEK_SET);
   
    write(fd, &value, 1);

    write(fd, &value, 1);

    write(fd, &value, 1);
    bytes = 0;
    while(bytes < (int)entry.nameLength) {
        write(fd, &value, 1);
        bytes++;
    }

     
}

void writeEntry(int fd, Entry_ext2 entry, unsigned int blockAddr) {
    lseek(fd, blockAddr, SEEK_SET);

    write(fd, &entry.inode, 4);
     
    write(fd, &entry.recLength, 2);
   
    write(fd, &entry.nameLength, 1);

    write(fd, &entry.fileType, 1);

    write(fd, entry.name, (int)entry.nameLength);

}


int searchFile(int fd, fs_ext2 ext, char* filename, int inode) {
    int found = 0;
    unsigned int inodeTable, inodeTableAddr, position;

    inodeTableAddr = getInodeTableAddr(inode);


    lseek(fd, SUPERBLOCK + SUPERBLOCK_SIZE + inodeTableAddr + 8, SEEK_SET);
    read(fd, &inodeTable, 4);

    position = (inode - 1)%ext.inodeInfo.inodesGroup;
    unsigned int root;

    if (inode == 2) root = inodeTable * ext.blockInfo.blockSize + ext.inodeInfo.inodeSize;
    else root = inodeTable * ext.blockInfo.blockSize + (ext.inodeInfo.inodeSize * position) + inodeTableAddr;

    unsigned int inodeSize; 
    lseek(fd, 4 + root, SEEK_SET);
    read(fd, &inodeSize, 4);

    unsigned int numEntries = inodeSize / ext.blockInfo.blockSize;
    unsigned int pointer, blockAddr = 0, index, access, prevBlockAddr;
    

    for (index = 0; index < numEntries; index++) {
        if (inode == 2) lseek(fd, (40 + (index * 4) + root), SEEK_SET);
        else lseek(fd, (40 + (index * ext.blockInfo.blockSize) + root), SEEK_SET);

        read(fd, &pointer, 4);

        if (pointer != 0) {
            blockAddr = pointer * ext.blockInfo.blockSize;
            int access = 0;

            while (access < inodeSize) {
                
                Entry_ext2 entry = readEntry(fd, blockAddr);
                
                if (entry.fileType & 0x01) {
                    if (strcmp(entry.name, filename) == 0) {
                        
                        removeEntry(fd, blockAddr, entry);
                        found = 1;
                       // break;
                        
                    }
                }
                else if (entry.fileType & 0x02) {
                    if (strcmp(entry.name, ".") != 0 && strcmp(entry.name, "..") != 0) {
                        //found = searchFile(fd, ext, filename, entry.inode);
                        
                    }
                }
                else if (found == 1) {
                    writeEntry(fd, entry, prevBlockAddr);
                    
                }
                prevBlockAddr = blockAddr;
                blockAddr += entry.recLength;
               

                access += entry.recLength;

                
                
            }
            
        }


    }


    return found;
}

void DELETE_fileExt2(int fd, fs_ext2 ext, char* filename) {
    int found = searchFile(fd, ext, filename, 2);
    char* buffer = (char*) malloc(sizeof(char));
    
    if (found == 1) {
        int bytes = sprintf(buffer, DEL_FILE, filename);
        write(1, buffer, bytes);
    }
    else write(1, FILE_NOT_FOUND, strlen(FILE_NOT_FOUND));
}

void RemoveFatEntry(int fd, unsigned int entryPosition) {

    unsigned char deletedFile = 0xE5;
    unsigned short newCluster = 0;

    lseek(fd, entryPosition, SEEK_SET);
    write(fd, &deletedFile, sizeof(unsigned char));

    lseek(fd, entryPosition + 11, SEEK_SET);
    write(fd, &deletedFile, sizeof(unsigned char));

    lseek(fd, entryPosition + 26, SEEK_SET);
    read(fd, &newCluster, sizeof(unsigned short));

}

void DELETE_fileFAT(int fd, fs_FAT fat, char* filename) {
    unsigned long fileposition = FIND_findFileFat(fd, fat, filename);

    char* buffer = (char*) malloc(sizeof(char));
    int bytes;

    if (fileposition != 0) {
        write(1, "\r\0", 2);
        RemoveFatEntry(fd, fileposition);
        bytes = sprintf(buffer, DEL_FILE, filename);
        write(1, buffer, bytes);
    }
}

