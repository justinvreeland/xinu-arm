#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>


/*
 * List all dirents
 */
//void ls( struct ext2_filesystem *fs, char *path ) {
void ls(char *path){

  if(!path){
    path = "./";
  }
    struct ext2_dir_entry_2 *dir = ext2_get_dirent_from_path( filesystem, path, "." );
    if (!dir) {
        printf("ls: %s: No such file or directory\n",path);
        return;
    }

    struct ext2_inode *dirInode = ext2_get_inode( filesystem, dir->inode );
    if (!dirInode) {
        printf("ls: %d: No such inode\n", dir->inode);
        return;
    }

    struct ext2_dir_entry_2 *firstDirent = ext2_get_first_dirent( filesystem, dirInode );
    struct ext2_dir_entry_2 *currDirent = firstDirent;

    while ( currDirent->inode &&
            ( currDirent - firstDirent < dirInode->i_size ) ) {
        printf( "%s\n", currDirent->name );
        currDirent = ext2_get_next_dirent( filesystem, currDirent );
    }
}

