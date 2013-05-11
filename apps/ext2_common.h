/* File: ext2_common.h
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Helper functions that both read and write use
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"
#include "ext2_read.h"

#ifndef _EXT2_COMMON_H
#define _EXT2_READ_H

/*
 * Return the size of a block in the fs with the given sb
 */
static inline uint32 get_block_size( struct ext2_super_block *sb )

{ return 1024 << sb->s_log_block_size; }

/*
 * Return the address of the given block number in the fs
 */
static inline void * block_num_to_addr( struct ext2_filesystem *fs,
                                              uint32 block_number )
{ return (void *) ( fs->base_addr + EXT2_SUPERBLOCK_LOCATION ) +
                    ( block_number * get_block_size( fs->sb ) ); }
/*
 * Get the next file entry
 */
static inline struct ext2_dir_entry_2 *
ext2_get_next_dirent( struct ext2_dir_entry_2 * dirent )

{ return ( (void *) dirent ) + dirent->rec_len; }

/*
 * Num of data blocks a single indirect block can point to
 */
static inline uint32 indirect_data_block_size( struct ext2_filesystem *fs )

{ return get_block_size( fs->sb ) / sizeof( uint32 ); }

/*
 * Return a pointer to the block num of the given index in the inode table
 */
static inline uint32* get_inode_block( struct ext2_filesystem *fs,
                                       struct ext2_inode *fp,
                                       uint32 logical_index ) {
    if ( logical_index >= EXT2_NDIR_BLOCKS +
                          EXT2_NIND_BLOCKS * indirect_data_block_size( fs ) ) {
        printf( "EXT2: No support for double/triple indirect blocks\n" );
        return 0;
    }
    if ( logical_index > EXT2_NDIR_BLOCKS ) {
        return (void *) ( block_num_to_addr( fs, fp->i_block[EXT2_NDIR_BLOCKS] ) +
                ( logical_index - EXT2_NDIR_BLOCKS ) );
    }
    return &( fp->i_block[logical_index] );
}

/*
 * Returns the inode with the given number (indexed at 1)
 */
struct ext2_inode * get_inode( struct ext2_filesystem *fs, uint32 inode_num );

/*
 * Prints the given dirent
 */
void print_dirent( struct ext2_dir_entry_2 * dirent );

/*
 * Returns a pointer to the superblock
 */
struct ext2_super_block * get_superblock( uint32 fs_start );

/*
 * Prints the given suberblock
 */
void print_superblock( struct ext2_super_block *sb );

/*
 * Prints the given inode
 */
void print_inode ( struct ext2_inode *in, int num, struct ext2_filesystem *fs );

/*
 * Converts the block number to the block group number holding that block
 */
static inline uint32 blk_to_blk_grp( struct ext2_filesystem *fs, uint32 blk_num ){
    return blk_num / fs->sb->s_blocks_per_group;
}

/*
 * Returns a pointer to the block bitmap for the block group which holds the
 * blk_num
 */
uint32 * get_blk_bitmap( struct ext2_filesystem *fs, uint32 blk_num );

/*
 * Returns a pointer to the inode bitmap for the block group
 */
uint32 * get_inode_bitmap( struct ext2_filesystem *fs,
                                uint32 inode_num );

/*
 * Returns 1 if the inode is used, 0 otherwise
 */
int inode_is_used( struct ext2_filesystem *fs, uint32 inode_num );

/*
 * Returns 1 if the block is used, 0 otherwise
 */
int blk_is_used( struct ext2_filesystem *fs, uint32 blk_num );

/*
 * Mark the given inode as occupied
 */
void mark_inode_used( struct ext2_filesystem *fs, uint32 inode_num );

/*
 * Mark the given block as occupied
 */
void mark_blk_used( struct ext2_filesystem *fs, uint32 blk_num );

/*
 * Allocate the first available block
 * Returns the block number or 0 if none are available
 */
uint32 blk_alloc( struct ext2_filesystem *fs );

/*
 * Adds blocks to the given inode to accomodate nbytes of data
 * returns the num successfully allocated
 */
uint32 increase_inode_size( struct ext2_filesystem *fs,
                                 struct ext2_inode *fp, uint32 nbytes );

#endif
