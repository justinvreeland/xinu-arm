/* * File: ext2.c
 * Author: Catherine Sullivan
 * Description: A command shell that forks a child process
 *  to 'exec' each command line entered by the user.
 *  if opened with a command line argument, it opens the ext2
 *  filesystem of that name
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"

# if 0
// TODO: Ext2 Superblock Operations
read_inode
write_inode
put_inode
delete_inode
put_super
write_super
statfs
remount_fs
#endif

#if 0
// TODO: Ext2 Inode Operations
ext2_create()
ext2_lookup()
ext2_link()
ext2_unlink()
ext2_symlink()
ext2_mkdir()
ext2_rmdir()
ext2_mknod()
ext2_rename()
#endif

#if 0
// TODO: Ext2 File Operations
generic_file_llseek()
generic_file_read()
generic_file_write()
ext2_ioclt()
generic_file_mmap()
generic_file_open()
ext2_release_file()
ext2_sync_file()
#endif

int ext2() {
    printf("Hello World, this is the Ext2 FS\n");
}
