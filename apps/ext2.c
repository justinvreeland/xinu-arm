/* File: ext2.c
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: A ram ext2 filesystem
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"
#include "ext2_write.h"
#include "ext2_read.h"
#include "ext2_common.h"

// Location of fs in xinu
struct ext2_filesystem *xinu_fs;
uint32 mem_lower_bound = MEM_LOWER_BOUND;

/*
 * Init the fs
 */
void _fs_ext2_init(void) {

    xinu_fs = (struct ext2_filesystem*)mem_lower_bound;
    mem_lower_bound += sizeof( struct ext2_filesystem );
    xinu_fs->sb = get_superblock( VIRT_MEM_LOCATION );
    xinu_fs->base_addr = (uint32) VIRT_MEM_LOCATION;

}

int ext2(void) {
    printf("Hello World, this is the Ext2 FS\n");

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
    blk5->rec_len = sizeof(struct ext2_dir_entry_2);
    blk5->name_len = 1;
    blk5->filetype = 2;
    char homeName[255] = ".";
    memcpy(blk5->name, homeName, 255);

    // Test the read/write functions
    _fs_ext2_init();
    printf("Testing hardcoded data\n");
    print_superblock( xinu_fs->sb );
    struct ext2_inode *i1 = get_inode(xinu_fs, 1);
    print_inode( i1, 1 );
    struct ext2_dir_entry_2 *home = ext2_get_first_dirent(xinu_fs, i1 );
    print_dirent( home );

    uint32 inode_num = ext2_inode_alloc( xinu_fs );
    struct ext2_inode *i2 = get_inode( xinu_fs, inode_num+1 );
    i2->i_mode = EXT2_S_IFREG;
    i2->i_size = 0;
    printf("Allocated new inode\n");
    print_inode( i2, inode_num+1 );

    struct ext2_dir_entry_2 *dirent = ext2_dirent_alloc( xinu_fs, i1 );
    dirent->inode = 2;
    dirent->rec_len = sizeof(struct ext2_dir_entry_2);
    dirent->name_len = 4;
    dirent->filetype = EXT2_FT_REG_FILE;
    char testName[255] = "test";
    memcpy(dirent->name, testName, 255);
    printf("Allocated new dir_entry_2 test\n");
    print_dirent( dirent );

    char path[8] = "./test";
    char buffer[14] = "Writing! Yay!";
    uint32 bytes_written;
    ext2_write_status stat = ext2_write_file_by_path( xinu_fs, path, buffer,
                                                      &bytes_written, 0, 13 );
    printf("bytes_written = %d stat = %d\n", bytes_written, stat);
    int read = 0;
    char readBuf[30];
    read = ext2_read_dirent( xinu_fs, dirent, readBuf, 0, 29);
    printf("Read %d bytes readBuf = %s\n", read, readBuf);

    return 0;
}
