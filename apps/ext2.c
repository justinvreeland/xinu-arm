/* File: ext2.c
 * Author: Catherine Sullivan
            Steven Shaw
 * Description: A ram ext2 filesystem
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"

// Location of fs in xinu
struct ext2_filesystem *xinu_fs;

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
 * Determine if the size of this dirent is too large (i.e. the last file entry)
 */
static inline int dirent_is_too_large( struct ext2_dir_entry_2 *dirent) {

    uint16 size = sizeof( struct ext2_dir_entry_2) + dirent->name_len;
    if (size %4)
        size += 4-(size %4);
    return size < dirent->rec_len;
}
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

struct ext2_super_block * ext2_get_superblock(uint32 fs_start);

/*
 * Init the fs
 */
void _fs_ext2_init() {

    xinu_fs = (struct ext2_filesystem*)mem_lower_bound;
    mem_lower_bound += sizeof( struct ext2_filesystem );
    xinu_fs->sb = ext2_get_superblock( VIRT_MEM_LOCATION );
    xinu_fs->base_addr = (uint32) VIRT_MEM_LOCATION;

}

/*
 * Returns a pointer to the first directory entry in the inode
 */
struct ext2_dir_entry_2 * ext2_get_first_dirent(struct ext2_filesystem *fs,
                                                struct ext2_inode * dir) {
    void * dir_data = block_num_to_addr( fs, dir->i_block[0]);
    struct ext2_dir_entry_2 * file;
    file = (struct ext2_dir_entry_2 *) dir_data;
    return file;
}

/*
 * Prints the given dirent
 */
void print_dirent(struct ext2_dir_entry_2 * dirent) {
    printf("Filename: %s\n", dirent->name);
    printf("\tInode: %d\n\r", dirent->inode);
    printf("\tEntry length %d\n", dirent->rec_len);
    printf("\tFile type %d\n", dirent->filetype);
}

/*
 * Returns a pointer to the superblock
 */
struct ext2_super_block * ext2_get_superblock( uint32 fs_start ) {

    struct ext2_super_block *sb;
    uint32 sb_start = fs_start + (EXT2_SUPERBLOCK_LOCATION);
    sb = (struct ext2_super_block *) sb_start;
    return sb;
}
/*
 * Prints the given suberblock
 */
void print_superblock( struct ext2_super_block *sb ) {

    printf("Printing Superblock Data\n");
    printf("\tVolume name: %s\n", sb->s_volume_name);
    printf("\tBlock size: = %d\n", 1024 << sb->s_log_block_size);
    printf("\t%d inodes\n", sb->s_inodes_count);
    printf("\t%d blocks\n", sb->s_blocks_count);
    printf("\t%d blocks reserved\n", sb->s_r_blocks_count);
    printf("\tFirst data block: %d\n", sb->s_first_data_block);
    printf("\tMagic number: %x\n", sb->s_magic);
}

/*
 * Returns the inode with the given number (indexed at 1)
 */
struct ext2_inode * ext2_get_inode( struct ext2_filesystem *fs, uint32 inode_num ) {

    struct ext2_super_block *sb = fs->sb;
    if ( inode_num > sb->s_inodes_count )
        printf("Requested inode num: %d out of bounds. Max num: %d\n",
               inode_num, sb->s_inodes_count);

    uint32 block_group = inode_num/sb->s_inodes_per_group;

    // Assume every blockgroup has a superblock for now
    uint32 iTable_offset = 3;

    void * iTable_addr = (void *) (fs->base_addr + EXT2_SUPERBLOCK_LOCATION
                                        + EXT2_SUPERBLOCK_SIZE
                                        + (iTable_offset * get_block_size(sb))
                                        + (block_group * sb->s_blocks_per_group));
    struct ext2_inode *inode = iTable_addr + ((inode_num-1)*sb->s_inode_size);
    return inode;
}

/*
 * Prints the given inode
 */
void print_inode (struct ext2_inode *in, int num) {
    printf("Printing inode number %d\n", num);
    printf("\ti_mode = 0x%x\n", in->i_mode);
    printf("\ti_size = %d\n", in->i_size);
    printf("\ti_flags = 0x%x\n", in->i_flags);
    printf("\tFirst block of data = %d\n", in->i_block[0]);
}

/*
 * Fills the given buffer with up to nbytes of data drom the given
 * directory entry and returns the number of bytes read.
 */
uint32 ext2_read_dirent (struct ext2_filesystem *fs, struct ext2_dir_entry_2 *file,
                           void *buffer, uint32 start, uint32 nbytes) {
    uint32 iNum = file->inode;
    struct ext2_inode *fp = ext2_get_inode(fs, iNum+1);
    uint32 block_size = get_block_size( fs->sb);
    // TO DO: IMPLEMENT INDIRECT BLOCKS

    // make sure buffer is big enough
    if (fp->i_size + start < (nbytes) ) {
        if (start > fp->i_size)
           nbytes = 0;
        else
            nbytes = fp->i_size - start;

    }
    uint32 remaining = nbytes;
    uint32 first_inode_block = start/block_size;
    uint32 block_offset = start % block_size;
    int i;
    for (i = first_inode_block; i < EXT2_NDIR_BLOCKS; i++) {
        uint32 block_number = fp->i_block[i];
        char *data = (char *) (block_offset +
                     block_num_to_addr(fs, block_number));
        if ( remaining < block_size ) {
            memcpy( buffer, data, remaining);
            remaining = 0;
            break;
        } else {
            memcpy( buffer, data, block_size-block_offset );
            buffer += (block_size-block_offset);
            remaining -= (block_size-block_offset);
        }

        // Only need to use on first block
        block_offset = 0;
    }
    return (nbytes - remaining);
}

int ext2() {
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
    blBitmap = (uint8 *) (gpd + 1024/sizeof(struct ext2_group_desc));
    blBitmap[0] = 0x7F;      // super block
    int i;
    for (i = 6; i < sb->s_blocks_count; i++)
        blBitmap[i] = 0;
    // Set up the inode bitmap
    uint8 *iBitmap;
    iBitmap = (uint8 *) (blBitmap + 1024/sizeof(uint8));
    iBitmap[0] = 0x3;     // .
    for (i = 1; i < sb->s_inodes_count; i++)
        iBitmap[i] = 0;

    // Set up the inode table
    struct ext2_inode *iTbl;
    iTbl = (struct ext2_inode *) (iBitmap + 1024/sizeof(uint8));
    // Set up . inode
    iTbl->i_mode = EXT2_S_IFDIR;
    iTbl->i_size = sizeof(struct ext2_dir_entry_2);
    iTbl->i_links_count = 0;
    iTbl->i_blocks = 1;
    iTbl->i_flags = EXT2_NODUMP_FL;
    iTbl->i_block[0] = 5;

    // Set up . entry for the home directory
    struct ext2_dir_entry_2 *blk6;
    blk6 = (struct ext2_dir_entry_2 *) (iTbl + 1024/sizeof(struct ext2_inode));
    blk6->inode = 0;
    blk6->rec_len = 2112;
    blk6->name_len = 2;
    blk6->filetype = 2;
    char homeName[255] = ".";
    memcpy(blk6->name, homeName, 255);

    // Set up test inode
    struct ext2_inode *i2 = iTbl+1;
    i2->i_mode = EXT2_S_IFREG;
    i2->i_size = 15;
    i2->i_links_count = 1;
    i2->i_blocks = 1;
    i2->i_flags = 0;
    i2->i_block[0] = 6;

    // Set up test entry for home directory
    struct ext2_dir_entry_2 * testDirent;
    testDirent = blk6 + 1;
    testDirent->inode = 1;
    testDirent->rec_len = 2112;
    testDirent->name_len = 5;
    testDirent->filetype = 1;
    char testName[255] = "test";
    memcpy(testDirent->name, testName, 255);

    // Write test data
    char test[15] = "Testing 1 2 3 ";
    void * blk7 = (void *) (iTbl + 2*1024/sizeof(struct ext2_inode));
    memcpy(blk7, test, 15);

    // Test the read functions written above
    _fs_ext2_init();
    print_superblock( xinu_fs->sb );
    struct ext2_inode * i1 = ext2_get_inode(xinu_fs, 1);
    print_inode( i1, 1 );
    struct ext2_dir_entry_2 * home = ext2_get_first_dirent(xinu_fs, i1 );
    print_dirent( home );
    char buffer[255];
    struct ext2_dir_entry_2 * testF = home + 1;
    struct ext2_inode * testi = ext2_get_inode(xinu_fs, testF->inode + 1);
    print_inode( testi, 2 );
    print_dirent( testF );
    int read = ext2_read_dirent( xinu_fs, testF, buffer, 0, 254 );
    printf("Read %d bytes buffer = %s\n", read, buffer);

    return 0;
}
