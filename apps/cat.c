#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>

/*
 * Print the contents of a file
 */
//void cat( struct ext2_filesystem *fs, char *path, char *name ) {
void cat( char *name){

    struct ext2_dir_entry_2 *file = ext2_get_dirent_from_path( filesystem, curdir, name );

    if (!file) {
        printf("cat: %s: No such file or directory\n", curdir);
        return;
    }

    struct ext2_inode *inode = ext2_get_inode( filesystem, file->inode );
    if (!inode) {
        printf("cat: %d: No such inode\n", file->inode);
        return;
    }

    char buffer[inode->i_size + 1];
    int read = ext2_read_dirent( filesystem, file, buffer, 0, inode->i_size );
    buffer[read] = 0;
    printf( "%s\n", buffer );
}

