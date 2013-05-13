/* File: lfsInit.c
 *
 * Author: Justin Vreeland
 *         Catherine Sulivan
 *         Stepven Shaw
 *
 * Description: Initilization of Rambased ext2 filessystem for Xinu
 */

#include <lfs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lfilesys.h>
#include "ext2.h"

struct lfdata Lf_data;

/*
 * Initializes the structures for the ext2 implementation
 *
 * assumes it's not going to fail, over writes absolutly everything that was
 * there.
 *
 * will be changed if SD care is implemented
 */
devcall lfsInit( struct dentry* dev){


  // Hardcode test fs into memory so that we can test read
  // Set up the superblock
  struct ext2_super_block *sb;
  sb = (struct ext2_super_block*) (VIRT_MEM_LOCATION +
      EXT2_SUPERBLOCK_LOCATION);
  sb->s_inodes_count          = 20;
  sb->s_blocks_count          = 50;
  sb->s_r_blocks_count        = 6;
  sb->s_free_blocks_count     = 43;
  sb->s_free_inodes_count     = 18;
  sb->s_first_data_block      = 1;
  sb->s_log_block_size        = 0;
  sb->s_log_frag_size         = 0;
  sb->s_blocks_per_group      = 50;
  sb->s_frags_per_group       = 50;
  sb->s_inodes_per_group      = 20;
  sb->s_magic                 = EXT2_MAGIC;
  sb->s_state                 = EXT2_VALID_FS;
  sb->s_errors                = EXT2_ERRORS_CONTINUE;
  sb->s_creator_os            = EXT2_OS_XINU;
  sb->s_first_ino             = 2;
  sb->s_inode_size            = sizeof(struct ext2_inode);
  sb->s_block_group_nr        = 0;
  char name[16]               = "FAKE RAM FS :D";
  memcpy(sb->s_volume_name,name,16);

  // Set up the group descriptors table
  struct ext2_group_desc *gpd;
  // DUMB POINTER ARITHMATIC
  gpd = (struct ext2_group_desc *) (sb + 1);
  gpd->bg_block_bitmap        = 2;
  gpd->bg_inode_bitmap        = 3;
  gpd->bg_inode_table         = 4;
  gpd->bg_free_blocks_count   = 44;
  gpd->bg_free_inodes_count   = 19;
  gpd->bg_used_dirs_count     = 1;

  // Set up the block bitmap
  uint8 *blBitmap;
  blBitmap = (uint8 *) (sb + 2);
  blBitmap[0] = 0x3F;      // super block
  int i;
  for (i = 6; i < sb->s_blocks_count; i++)
    blBitmap[i] = 0;
  // Set up the inode bitmap
  uint8 *iBitmap;
  iBitmap = (uint8 *) (sb + 3);
  iBitmap[0] = 0x1;     // .
  for (i = 1; i < sb->s_inodes_count; i++)
    iBitmap[i] = 0;

  // Set up the inode table
  struct ext2_inode *iTbl;
  iTbl = (struct ext2_inode *) (sb + 4);
  // Set up . inode
  iTbl->i_mode = EXT2_S_IFDIR;
  iTbl->i_size = sizeof(struct ext2_dir_entry_2);
  iTbl->i_links_count = 0;
  iTbl->i_blocks = 1;
  iTbl->i_flags = EXT2_NODUMP_FL;
  iTbl->i_block[0] = 5;

  // Set up . entry for the home directory
  struct ext2_dir_entry_2 *blk5;
  blk5 = (struct ext2_dir_entry_2 *) (sb + 5);
  blk5->inode = 1;
  blk5->next_dirent = 0; 
  blk5->name_len = 1;
  blk5->filetype = 2;
  char homeName[255] = ".";
  memcpy(blk5->name, homeName, 255);

  // add to device data entry thingy
  Lf_data.xinufs = (struct ext2_filesystem*)(MEM_LOWER_BOUND);
  Lf_data.xinufs->sb = get_superblock( VIRT_MEM_LOCATION );
  Lf_data.xinufs->base_addr =(uint32) VIRT_MEM_LOCATION;

  printf("We have done OK: %d\n", OK);
  return OK;

}
