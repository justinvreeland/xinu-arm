/* File: ext2_read.c
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: The read functionality for the ext2
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ext2.h"
#include "ext2_read.h"
#include "ext2_common.h"

/*
 * Returns a pointer to the first directory entry in the inode
 */
struct ext2_dir_entry_2 * ext2_get_first_dirent( struct ext2_filesystem *fs,
                                                struct ext2_inode * dir ) {
    void * dir_data = block_num_to_addr( fs, dir->i_block[0] );
    struct ext2_dir_entry_2 * file;
    file = (struct ext2_dir_entry_2 *) dir_data;
    return file;
}

/*
 * Get the next file entry
 */
struct ext2_dir_entry_2 * ext2_get_next_dirent( struct ext2_filesystem *fs,
                                            struct ext2_dir_entry_2 *dirent ) {
    return dirent->next_dirent;

}

/*
 * Prints the given dirent
 */
void print_dirent( struct ext2_dir_entry_2 * dirent ) {
    printf( "Filename: %s\n", dirent->name );
    printf( "\tInode: %d\n\r", dirent->inode );
    printf( "\tNext Dirent %d\n", dirent->next_dirent );
    printf( "\tFile type %d\n", dirent->filetype );
}

/*
 * Returns a pointer to the superblock
 */
struct ext2_super_block * get_superblock( uint32 fs_start ) {

    struct ext2_super_block *sb;
    uint32 sb_start = fs_start + ( EXT2_SUPERBLOCK_LOCATION );
    sb = (struct ext2_super_block *) sb_start;
    return sb;
}
/*
 * Prints the given suberblock
 */
void print_superblock( struct ext2_super_block *sb ) {

    printf( "Printing Superblock Data\n" );
    printf( "\tVolume name: %s\n", sb->s_volume_name );
    printf( "\tBlock size: = %d\n", 1024 << sb->s_log_block_size );
    printf( "\t%d inodes\n", sb->s_inodes_count );
    printf( "\t%d blocks\n", sb->s_blocks_count );
    printf( "\t%d blocks reserved\n", sb->s_r_blocks_count );
    printf( "\tFirst data block: %d\n", sb->s_first_data_block );
    printf( "\tMagic number: %x\n", sb->s_magic );
}

/*
 * Returns the inode with the given number (indexed at 1)
 */
struct ext2_inode * ext2_get_inode( struct ext2_filesystem *fs, uint32 inode_num ) {

    struct ext2_super_block *sb = fs->sb;
    if ( inode_num > sb->s_inodes_count ) {
        printf( "Requested inode num: %d out of bounds. Max num: %d\n",
               inode_num, sb->s_inodes_count );
        return 0;
    }
    uint32 block_group = inode_num/sb->s_inodes_per_group;

    // Assume every blockgroup has a superblock for now
    uint32 iTable_offset = 3;

    void * iTable_addr = (void *) ( fs->base_addr + EXT2_SUPERBLOCK_LOCATION
                                        + EXT2_SUPERBLOCK_SIZE
                                        + ( iTable_offset * get_block_size( sb ) )
                                        + ( block_group * sb->s_blocks_per_group ) );
    struct ext2_inode *inode = iTable_addr + ( ( inode_num-1 ) * sb->s_inode_size );
    return inode;
}

/*
 * Prints the given inode
 */
void print_inode( struct ext2_inode *in, int num, struct ext2_filesystem *fs ) {
    printf( "Printing inode number %d\n", num );
    printf( "\ti_mode = 0x%x\n", in->i_mode );
    printf( "\ti_size = %d\n", in->i_size );
    printf( "\ti_flags = 0x%x\n", in->i_flags );
    printf( "\tFirst block of data = %d\n", in->i_block[0] );
    int i = 1;
    while ( in->i_block[i] ) {
        printf( "\tBlock %d of data = %d\n",i, in->i_block[i] );
        i++;
    }
    if ( in->i_block[EXT2_NDIR_BLOCKS] ) {
        int i = 0;
        uint32 *blk = block_num_to_addr( fs, in->i_block[EXT2_NDIR_BLOCKS] );
        while ( *(blk + i) ) {
            printf( " \tIndirect Block %d of data = %d\n", i, *( blk + i ) );
            i++;
        }
    }
}

/*
 * Fills the given buffer with up to nbytes of data drom the given
 * directory entry and returns the number of bytes read.
 */
uint32 ext2_read_dirent ( struct ext2_filesystem *fs,
                         struct ext2_dir_entry_2 *file,
                         void *buffer, uint32 start, uint32 nbytes ) {
    uint32 iNum = file->inode;
    struct ext2_inode *fp = ext2_get_inode( fs, iNum );
    uint32 block_size = get_block_size( fs->sb );

    // make sure buffer is big enough
    if ( fp->i_size + start < ( nbytes ) ) {
        if ( start > fp->i_size )
           nbytes = 0;
        else
            nbytes = fp->i_size - start;

    }
    uint32 remaining = nbytes;
    uint32 first_inode_block = start/block_size;
    uint32 block_offset = start % block_size;
    int i;
    char *data;
    for ( i = first_inode_block; i < EXT2_N_BLOCKS; i++ ) {
        if ( i < EXT2_NDIR_BLOCKS ) {
            uint32 block_number = fp->i_block[i];
            data = (char *)
                   ( block_offset + block_num_to_addr(fs, block_number ) );
        } else {
            uint32 indir_blk_num = fp->i_block[EXT2_NDIR_BLOCKS];
            uint32 block_number = *( (uint32 *)
                                     block_num_to_addr( fs, indir_blk_num ) +
                                     i - EXT2_NDIR_BLOCKS );
            data = (char *) (block_offset +
                    block_num_to_addr( fs, block_number ) );
        }
        if ( remaining < block_size ) {
            memcpy( buffer, data, remaining );
            remaining = 0;
            break;
        } else {
            memcpy( buffer, data, block_size-block_offset );
            buffer += ( block_size-block_offset );
            remaining -= ( block_size-block_offset );
        }

        // Only need to use on first block
        block_offset = 0;
    }
    return (nbytes - remaining);
}

/*
 * Returns the dirent with the given name from the directory's inode
 * 0 if it does not exist
 */
struct ext2_dir_entry_2* ext2_get_dirent_from_inode (struct ext2_filesystem *fs,
                                                    struct ext2_inode *dir_ino,
                                                    const char *filename) {
    uint32 filename_len = strnlen( filename, EXT2_NAME_LEN);
    uint32 block_size = get_block_size( fs->sb );
    int i;
    uint32 block_num;
    for (i = 0; i < EXT2_N_BLOCKS; i++) {
        if ( i < EXT2_NDIR_BLOCKS ) {
            block_num = dir_ino->i_block[i];
        }
        else {
            uint32 indir_blk_num = dir_ino->i_block[EXT2_NDIR_BLOCKS];
            block_num = *( (uint32 *)
                                  block_num_to_addr( fs, indir_blk_num ) +
                                  i - EXT2_NDIR_BLOCKS );
        }
        struct ext2_dir_entry_2 *dirent = (struct ext2_dir_entry_2 *)
                                           block_num_to_addr(fs, block_num);
        struct ext2_dir_entry_2 *first_dirent = dirent;
        int go_to_next_block = 0;
        while ( dirent ) {
            if (filename_len == dirent->name_len &&
                !strncmp( filename, dirent->name, dirent->name_len )) {
                return dirent;
            } else {
                dirent = ext2_get_next_dirent( fs, dirent );
                if ( dirent > (first_dirent + block_size) ) {
                    go_to_next_block = 1;
                    break;
                }
            }
        }
        if ( !go_to_next_block )
            break;
    }
    return 0;
}

/*
 * Returns the dirent with the given path, 0 if it does not exist
 */
struct ext2_dir_entry_2 * ext2_get_dirent_from_path( struct ext2_filesystem *fs,
                                                     const char *dirpath,
                                                     const char *filename ) {
    if ( dirpath[0] == DIR_SEP )
        dirpath++;
    uint dir_len = strchr( dirpath, DIR_SEP ) - dirpath;

    char curr_dir_name[strnlen( dirpath, EXT2_NAME_LEN ) +1];

    struct ext2_inode *curr_inode;
    curr_inode = ext2_get_inode( fs, EXT2_INODE_ROOT );

    struct ext2_dir_entry_2 *curr_dirent =
        ext2_get_dirent_from_inode( fs, curr_inode, "." );

    while (*dirpath) {
        memcpy( curr_dir_name, dirpath, dir_len );
        curr_dir_name[dir_len] = 0;
        dirpath += dir_len+1;
        curr_dirent = ext2_get_dirent_from_inode( fs, curr_inode, curr_dir_name );
        // Didn't find the file or directory
        if ( !curr_dirent )
            return 0;
        dir_len = strchr( dirpath, DIR_SEP ) - dirpath;
        curr_inode = ext2_get_inode( fs, curr_dirent->inode );
    }
    struct ext2_inode *dirent_inode = ext2_get_inode( fs, curr_dirent->inode );
    return ext2_get_dirent_from_inode( fs, dirent_inode, filename );
}

/*
 * Fills the given buffer with up to nbytes of data drom the file with the given
 * path and returns the number of bytes read.
 */
uint32 ext2_read_file_from_path( struct ext2_filesystem *fs, char *path,
                          char *filename, void *buffer,
                          uint32 start, uint32 nbytes ) {

    struct ext2_dir_entry_2 *file = ext2_get_dirent_from_path( fs, path, filename );
    if ( !file ) {
        return 0;
    }
    return ext2_read_dirent( fs, file, buffer, start, nbytes );
}
