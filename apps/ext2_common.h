/* File: ext2_common.h
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Helper functions that both read and write use
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"

/*
 * Return the size of a block in the fs with the given sb
 */
static inline uint32 get_block_size( struct ext2_super_block *sb )

{ return 1024 << sb->s_log_block_size; }

/*
 * Return the address of the given block number in the fs
 */
static inline void * block_num_to_addr( struct ext2_filesystem *fs,
                                              uint32 block_number)
{ return (void *) (fs->base_addr + EXT2_SUPERBLOCK_LOCATION) +
                    (block_number * get_block_size(fs->sb)); }
/*
 * Get the next file entry
 */
static inline struct ext2_dir_entry_2 *
get_next_dirent( struct ext2_dir_entry_2 * dirent)

{ return ((void *) dirent) + dirent->rec_len; }

/*
 * Num of data blocks a single indirect block can point to
 */
static inline uint32 indirect_data_block_size( struct ext2_filesystem *fs)

{ return get_block_size( fs->sb ) / sizeof(uint32); }


/*
 * Return a pointer to the block of the given index in the inode table
 */
static inline uint32* get_inode_block ( struct ext2_inode *fp,
                                        uint32 logical_index ) {
    if (logical_index >= 12)
        printf("EXT2: No support for indirect blocks.\n");
    return &(fp->i_block[logical_index]);
}

