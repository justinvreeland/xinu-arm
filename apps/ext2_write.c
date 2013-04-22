/* File: ext2_write.c
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Write functionality for the ext2
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2_write.h"
#include "ext2_common.h"

/*
 * Returns a pointer to the block bitmap for the block group which holds the
 * blk_num
 */
uint32 * get_blk_bitmap(struct ext2_filesystem *fs, uint32 blk_num) {
    uint32 grp_desc_tbl_blk_num = 2;
    uint32 blk_grp_num = blk_to_blk_grp(fs, blk_num);

    struct ext2_group_desc *grp_desc_tbl =
            block_num_to_addr(fs, grp_desc_tbl_blk_num);

    uint32 blk_bitmap_blk_num = grp_desc_tbl[blk_grp_num].bg_block_bitmap;

    uint32 *blk_bitmap = (uint32 *) block_num_to_addr(fs, blk_bitmap_blk_num);

    return blk_bitmap;
}
