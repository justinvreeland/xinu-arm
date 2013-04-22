/* File: ext2_read.h
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Header file for the ext2 read functions
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"

#ifndef _EXT2_READ_H
#define _EXT2_READ_H

/*
 * Returns a pointer to the first directory entry in the inode
 */
struct ext2_dir_entry_2 * ext2_get_first_dirent(struct ext2_filesystem *fs,
                                                struct ext2_inode * dir);
/*
 * Prints the given dirent
 */
void print_dirent(struct ext2_dir_entry_2 * dirent);

/*
 * Returns a pointer to the superblock
 */
struct ext2_super_block * ext2_get_superblock( uint32 fs_start );

/*
 * Prints the given suberblock
 */
void print_superblock( struct ext2_super_block *sb );

/*
 * Returns the inode with the given number (indexed at 1)
 */
struct ext2_inode * ext2_get_inode( struct ext2_filesystem *fs, uint32 inode_num );

/*
 * Prints the given inode
 */
void print_inode (struct ext2_inode *in, int num);

/*
 * Fills the given buffer with up to nbytes of data drom the given
 * directory entry and returns the number of bytes read.
 */
uint32 ext2_read_dirent (struct ext2_filesystem *fs,
                         struct ext2_dir_entry_2 *file,
                         void *buffer, uint32 start, uint32 nbytes);

/*
 * Returns the dirent with the given name from the directory's inode
 * 0 if it does not exist
 */
struct ext2_dir_entry_2* ext2_get_dirent_from_inode (struct ext2_filesystem *fs,
                                                    struct ext2_inode *dir_ino,
                                                    const char *filename);

/*
 * Returns the dirent with the given path, 0 if it does not exist
 */
struct ext2_dir_entry_2 * ext2_get_dirent_from_path( struct ext2_filesystem *fs,
                                                     const char *dirpath,
                                                     const char *filename );

#endif