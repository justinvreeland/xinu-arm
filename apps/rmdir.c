#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>

//void rmdir1( struct ext2_filesystem *fs, char *path, char *dirName ) {
void rmdir(char* dirName){

    struct ext2_dir_entry_2 *parentDirent = ext2_get_dirent_from_path( filesystem, curdir, "." );
    struct ext2_dir_entry_2 *dirent = ext2_get_dirent_from_path( filesystem, curdir, dirName );

    ext2_dirent_dealloc( filesystem, dirent, parentDirent->inode );

    struct ext2_inode *inode = ext2_get_inode( filesystem, dirent->inode );
    struct ext2_dir_entry_2 *first_dirent = ext2_get_first_dirent( filesystem, inode );
    struct ext2_dir_entry_2 *curr_dirent = first_dirent;

    while( curr_dirent ) {
        ext2_inode_dealloc( filesystem, curr_dirent->inode );
        curr_dirent = ext2_get_next_dirent( filesystem, curr_dirent );
    }

    ext2_inode_dealloc( filesystem, first_dirent->inode );

}

