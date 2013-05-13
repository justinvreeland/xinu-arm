#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>

//void copy1( struct ext2_filesystem *fs, char *pathS,
           //char *nameS, char *curdir, char *nameD ) {
void cp( char* nameS, char* nameD){

    struct ext2_dir_entry_2 *direntS = ext2_get_dirent_from_path( filesystem, curdir, nameS );
    if (!direntS) {
        printf("copy: %s%s: file not found\n", curdir, nameS);
        return;
    }
    struct ext2_dir_entry_2 *parentDirentD = ext2_get_dirent_from_path( filesystem, curdir, "." );
    if (!parentDirentD) {
        printf("copy: %s: directory not found\n", curdir);
        return;
    }
    struct ext2_inode *parentInode = ext2_get_inode( filesystem, parentDirentD->inode );
    if (!parentInode) {
        printf("copy: %d: inode not found\n", parentDirentD->inode);
        return;
    }
    struct ext2_inode *inodeS = ext2_get_inode( filesystem, direntS->inode );
    if (!inodeS) {
        printf("copy: %d: inode not found\n", direntS->inode);
        return;
    }

    uint32 newInodeNum = ext2_inode_alloc( filesystem );
    if (!newInodeNum) {
        printf("copy: no new inode available\n");
        return;
    }
    struct ext2_inode *newInode = ext2_get_inode( filesystem, newInodeNum );
    if (!newInode)
        return;

    newInode->i_mode = inodeS->i_mode;
    newInode->i_size = 0;

    struct ext2_dir_entry_2 *newDirent = ext2_dirent_alloc( filesystem, parentInode );
    if (!newDirent) {
        printf("copy: no new dirents available\n");
        return;
    }
    newDirent->inode = newInodeNum;
    newDirent->next_dirent = 0;
    newDirent->name_len = strnlen( nameD, EXT2_NAME_LEN );
    newDirent->filetype = direntS->filetype;
    memcpy( newDirent->name, nameD, strnlen( nameD, EXT2_NAME_LEN ) );

    char buff[inodeS->i_size];
    int bytesRead  = ext2_read_dirent( filesystem, direntS, buff, 0, inodeS->i_size );
    int bytesWritten = ext2_write_file_by_inode( filesystem, newDirent, buff, 0, bytesRead );

}

