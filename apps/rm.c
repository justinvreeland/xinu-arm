#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>

/*
 * Remove a file
 */
void rm( struct ext2_filesystem *fs, char *path, char *name ) {

    struct ext2_dir_entry_2 *file = ext2_get_dirent_from_path( fs, path, name );
    if (!file) {
        return;
    }
    if ( file->filetype == EXT2_FT_DIR ) {
        printf("rm: %s%s: is a directory\n", path, name);
        return;
    }
    struct ext2_dir_entry_2 *dir = ext2_get_dirent_from_path( fs, path, "." );
    ext2_dirent_dealloc( fs, file, dir->inode );
    ext2_inode_dealloc( fs, file->inode );
}
