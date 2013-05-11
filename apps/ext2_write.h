/* File: ext2_write.h
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Header file for ext2 write functions
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"

#ifndef _EXT2_WRITE_H

typedef enum {
    EXT2_WRITE_SUCCESS = 0,
    EXT2_WRITE_FILE_NOT_FOUND = 1,
    EXT2_WRITE_NON_CONTIGUOUS = 2
} ext2_write_status;

/*
 * Allocate the first available inode
 * Returns the number of the inode or 0 if none are available
 */
uint32 ext2_inode_alloc( struct ext2_filesystem *fs );

/*
 * Allocate a new directory entry for the given directory inode
 * Returns a pointer to the new dirent
 */
struct ext2_dir_entry_2 * ext2_dirent_alloc( struct ext2_filesystem *fs,
                                             struct ext2_inode *inode );

/*
 * Write nybtes of the buffer into the file starting start bytes in
 * returns number of bytes actually written
 */
uint32 ext2_write_file_by_inode( struct ext2_filesystem *fs,
                                 struct ext2_dir_entry_2 *file,
                                 void *buffer, uint32 start, uint32 nbytes );

/*
 * Write nbytes from buffer into the file given by path starting start bytes in
 * Returns a write_status enum
 */
ext2_write_status ext2_write_file_by_path( struct ext2_filesystem *fs,
                                           const char *path,
                                           const char *buffer,
                                           uint32 *bytes_written,
                                           uint32 start,
                                           uint32 nbytes );
#endif
