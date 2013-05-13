#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>

/*
 * Remove a file
 */
//void rm( struct ext2_filesystem *fs, char *path, char *name ) {
void rm(char *name){

    struct ext2_dir_entry_2 *file = ext2_get_dirent_from_path( filesystem, curdir, name );
    if (!file) {
        return;
    }
    if ( file->filetype == EXT2_FT_DIR ) {
        printf("rm: %s%s: is a directory\n", curdir, name);
        return;
    }
    struct ext2_dir_entry_2 *dir = ext2_get_dirent_from_path( filesystem, curdir, "." );
    ext2_dirent_dealloc( filesystem, file, dir->inode );
    ext2_inode_dealloc( filesystem, file->inode );
}
