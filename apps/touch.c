#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>

/*
 * Create a new empty file of the given name in the given path
 */
void touch( struct ext2_filesystem *fs, char *path, char *name ) {

    struct ext2_dir_entry_2 *dir = ext2_get_dirent_from_path( fs, path, "." );
    if (!dir) {
        printf("touch: %s: No such file or directory\n",path);
        return;
    }

    struct ext2_inode *dirInode = ext2_get_inode( fs, dir->inode );
    if (!dirInode) {
        printf("touch: %d: No such inode\n", dir->inode);
        return;
    }
    uint32 newInodeNum = ext2_inode_alloc( fs );
    if (!newInodeNum) {
        printf("touch: No inode available\n");
        return;
    }

    struct ext2_inode *newInode = ext2_get_inode( fs, newInodeNum );
    if (!newInode) {
        printf("touch: %d: No such inode\n", newInodeNum);
        return;
    }
    newInode->i_mode = EXT2_S_IFREG;
    newInode->i_size = 0;

    struct ext2_dir_entry_2 *newDirent = ext2_dirent_alloc( fs, dirInode );

    if (!newDirent) {
        printf("touch: No dirent available\n");
        return;
    }
    newDirent->inode = newInodeNum;
    memcpy( newDirent->name, name, strnlen( name, EXT2_NAME_LEN ) );
    newDirent->next_dirent = 0;
    newDirent->name_len = strnlen( name, EXT2_NAME_LEN );
    newDirent->filetype = EXT2_FT_REG_FILE;

}

