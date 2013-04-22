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

/*
 * Converts the block number to the block group number holding that block
 */
static inline uint32 blk_to_blk_grp(struct ext2_filesystem *fs, uint32 blk_num){
    return blk_num / fs->sb->s_blocks_per_group;
}

/*
 * Returns a pointer to the block bitmap for the block group which holds the
 * blk_num
 */
uint32 * get_blk_bitmap(struct ext2_filesystem *fs, uint32 blk_num);

#endif
