/* File: ext2_write.c
 * Author: Catherine Sullivan
 *           Steven Shaw
 * Description: Write functionality for the ext2
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ext2_write.h"
#include "ext2_common.h"

/*
 * Returns a pointer to the block bitmap for the block group which holds the
 * blk_num
 */
uint32 * get_blk_bitmap( struct ext2_filesystem *fs, uint32 blk_num ) {

    uint32 grp_desc_tbl_blk_num = 1;
    uint32 blk_grp_num = blk_to_blk_grp( fs, blk_num );

    struct ext2_group_desc *grp_desc_tbl =
            block_num_to_addr( fs, grp_desc_tbl_blk_num );

    uint32 blk_bitmap_blk_num = grp_desc_tbl[blk_grp_num].bg_block_bitmap;

    uint32 *blk_bitmap = (uint32 *) block_num_to_addr( fs, blk_bitmap_blk_num );

    return blk_bitmap;
}

/*
 * Returns a pointer to the inode bitmap for the block group
 */
uint32 * get_inode_bitmap( struct ext2_filesystem *fs,
                                uint32 inode_num ) {

    uint32 grp_desc_tbl_blk_num = 1;
    uint32 blk_grp_num = inode_num/fs->sb->s_inodes_per_group;
    struct ext2_group_desc *grp_desc_tbl =
            block_num_to_addr( fs, grp_desc_tbl_blk_num );

    uint32 inode_bitmap_blk_num = grp_desc_tbl[blk_grp_num].bg_inode_bitmap;

    uint32 *inode_bitmap = (uint32 *)
                            block_num_to_addr( fs, inode_bitmap_blk_num );

    return inode_bitmap;
}

/*
 * Returns 1 if the block is used, 0 otherwise
 */
int blk_is_used( struct ext2_filesystem *fs, uint32 blk_num ) {

    uint32 *blk_bitmap = get_blk_bitmap( fs, blk_num );
    uint32 blk_bitmap_index = ( blk_num % fs->sb->s_blocks_per_group )/32;
    uint32 used = blk_bitmap[blk_bitmap_index] & 1 << ( blk_num % 32 );

    if ( used ) used = 1;

    return used;
}

/*
 * Returns 1 if the inode is used, 0 otherwise
 */
int inode_is_used( struct ext2_filesystem *fs, uint32 inode_num ) {

    uint32 *inode_bitmap = get_inode_bitmap( fs, inode_num );
    uint32 inode_bitmap_index = ( inode_num % fs->sb->s_inodes_per_group )/32;
    uint32 used = inode_bitmap[inode_bitmap_index] & 1 << ( inode_num % 32 );

    if ( used ) used = 1;

    return used;
}

/*
 * Mark the given block as occupied
 */
void mark_blk_used( struct ext2_filesystem *fs, uint32 blk_num ) {

    uint32 *blk_bitmap = get_blk_bitmap( fs, blk_num );
    uint32 blk_bitmap_index = ( blk_num % fs->sb->s_blocks_per_group )/32;

    blk_bitmap[blk_bitmap_index] |= 1 << ( blk_num % 32 );

    fs->sb->s_free_blocks_count --;

    uint32 desc_tbl_blk_num = 1;
    uint32 blk_grp_num = blk_to_blk_grp(fs, blk_num);
    struct ext2_group_desc *desc_tbl = block_num_to_addr( fs, desc_tbl_blk_num );
    desc_tbl[blk_grp_num].bg_free_blocks_count --;
}

/*
 * Mark the given block as unoccupied
 */
void mark_blk_unused( struct ext2_filesystem *fs, uint32 blk_num ) {

    uint32 *blk_bitmap = get_blk_bitmap( fs, blk_num );
    uint32 blk_bitmap_index = ( blk_num % fs->sb->s_blocks_per_group )/32;

    blk_bitmap[blk_bitmap_index] &= 0 << ( blk_num % 32 );

    fs->sb->s_free_blocks_count --;

    uint32 desc_tbl_blk_num = 1;
    uint32 blk_grp_num = blk_to_blk_grp(fs, blk_num);
    struct ext2_group_desc *desc_tbl = block_num_to_addr( fs, desc_tbl_blk_num );
    desc_tbl[blk_grp_num].bg_free_blocks_count --;
}

/*
 * Mark the given inode as occupied
 */
void mark_inode_used( struct ext2_filesystem *fs, uint32 inode_num ) {

    uint32 *inode_bitmap = get_inode_bitmap( fs, inode_num );
    uint32 inode_bitmap_index = ( inode_num % fs->sb->s_inodes_per_group )/32;

    inode_bitmap[inode_bitmap_index] |= 1 << ( inode_num % 32 );

    fs->sb->s_free_inodes_count --;

    uint32 desc_tbl_blk_num = 1;
    uint32 blk_grp_num = inode_num/fs->sb->s_inodes_per_group;
    struct ext2_group_desc *desc_tbl = block_num_to_addr( fs, desc_tbl_blk_num );
    desc_tbl[blk_grp_num].bg_free_inodes_count --;
}

/*
 * Mark the given inode as unoccupied
 */
void mark_inode_unused( struct ext2_filesystem *fs, uint32 inode_num ) {

    uint32 *inode_bitmap = get_inode_bitmap( fs, inode_num );
    uint32 inode_bitmap_index = ( inode_num % fs->sb->s_inodes_per_group )/32;

    inode_bitmap[inode_bitmap_index] &= 0 << ( inode_num % 32 );

    fs->sb->s_free_inodes_count --;

    uint32 desc_tbl_blk_num = 1;
    uint32 blk_grp_num = inode_num/fs->sb->s_inodes_per_group;
    struct ext2_group_desc *desc_tbl = block_num_to_addr( fs, desc_tbl_blk_num );
    desc_tbl[blk_grp_num].bg_free_inodes_count --;
}

/*
 * Allocate the first available block
 * Returns the block number or 0 if none are available
 */
uint32 blk_alloc( struct ext2_filesystem *fs ) {

    uint32 i;

    for ( i = 0; i<fs->sb->s_blocks_count; i++ ) {
        if ( !blk_is_used( fs, i ) ) {
            mark_blk_used( fs, i );
            return i;
        }
    }
    // everything is used
    return 0;
}

/*
 * Allocate the first available inode
 * Returns the number of the inode or 0 if none are available
 */
uint32 ext2_inode_alloc( struct ext2_filesystem *fs ) {

    uint32 i;

    for ( i = 0; i<fs->sb->s_inodes_count; i++ ) {
        if ( !inode_is_used( fs, i ) ) {
            mark_inode_used( fs, i );
            return i+1;
        }
    }
    // everything is used
    return 0;
}

/*
 * Deallocates the given inode and its blocks
 */
void ext2_inode_dealloc( struct ext2_filesystem *fs, uint32 inode_num ) {

    struct ext2_inode *inode = ext2_get_inode( fs, inode_num );
    if (!inode)
        return;

    int i;
    for ( i = 0; i < inode->i_blocks; i++ )
        mark_blk_unused( fs, inode->i_block[i] );

    mark_inode_unused( fs, inode_num );
}

/*
 * Allocate a new directory entry for the given directory inode
 * Returns a pointer to the new dirent
 */
struct ext2_dir_entry_2 * ext2_dirent_alloc( struct ext2_filesystem *fs,
                                             struct ext2_inode *inode ) {

    struct ext2_dir_entry_2 *start = ext2_get_first_dirent( fs, inode );
    struct ext2_dir_entry_2 *dirent = start;
    uint32 size = inode->i_size;
    // Allocate space for new dirent
    uint32 blocks_added = increase_inode_size( fs, inode,
                                     sizeof( struct ext2_dir_entry_2 ) );
    if ( inode->i_size != (size + sizeof( struct ext2_dir_entry_2 )) )
        return 0;

    // Now that the inode has enough space for sure, find the first open dirent
    int i;
    for ( i = 0; i < inode->i_blocks; i++ ) {
        // While the current dirent is occupied
        while ( dirent && dirent->inode ) {
            dirent = ext2_get_next_dirent( fs, dirent, inode );
        }
    }
    return dirent;
}

/*
 * Deallocate the given dirent
 */
void ext2_dirent_dealloc( struct ext2_dir_entry_2 *dirent) {

    dirent->inode = 0;
    dirent->rec_len = 0;
    dirent->name_len = 0;
    dirent->filetype = 0;
    dirent->name[0] = 0;

}

/*
 * Adds blocks to the given inode to accomodate nbytes of data
 * returns the num successfully allocated
 */
uint32 increase_inode_size( struct ext2_filesystem *fs,
                                 struct ext2_inode *fp, uint32 nbytes ) {

    uint32 blk_size = get_block_size( fs->sb );
    uint32 init_size = fp->i_size;
    uint32 blk_index = init_size / blk_size;

    uint32 blks_alloc = 0;
    uint32 total_bytes = nbytes;

    if ( init_size % blk_size ) {
        int decr = blk_size - ( init_size % blk_size );
        if ( decr > nbytes )
            nbytes = 0;
        else
            nbytes -= decr;
        blk_index ++;
    }

    uint32 num_blks_to_alloc = nbytes / blk_size;
    if ( nbytes % blk_size ) {
        num_blks_to_alloc += 1;
    }

    int last_blk_alloc = 0;
    uint i;
    uint32 *inode_blk;
    for ( i = 0; i< num_blks_to_alloc; i++ ) {
        last_blk_alloc = blk_alloc( fs );
        if ( last_blk_alloc ) {
            blks_alloc ++;
            if ( blk_index + i < EXT2_NDIR_BLOCKS ) {
                inode_blk = get_inode_block( fs, fp, blk_index + i );
                *inode_blk = last_blk_alloc;
            } else if ( blk_index + i == EXT2_NDIR_BLOCKS ) {
                inode_blk = get_inode_block( fs, fp, EXT2_NDIR_BLOCKS );
                *inode_blk = blk_alloc( fs );
                //Out of space
                if (!inode_blk){
                    blks_alloc --;
                    break;
                }
                uint32 *indir_blk = block_num_to_addr( fs, *inode_blk );
                *( indir_blk + blk_index + i - EXT2_NDIR_BLOCKS ) = last_blk_alloc;
            } else if ( blk_index + i < EXT2_N_BLOCKS ){
                inode_blk = get_inode_block( fs, fp, EXT2_NDIR_BLOCKS );
                uint32 *indir_blk = block_num_to_addr( fs, *inode_blk );
                *( indir_blk + i - EXT2_NDIR_BLOCKS ) = last_blk_alloc;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    if ( blks_alloc == num_blks_to_alloc ) {
        fp->i_size += total_bytes;
    } else {
        fp->i_size += total_bytes -
                    ( ( num_blks_to_alloc - blks_alloc )*blk_size) ;
    }

    return blks_alloc;
}

/*
 * Write nybtes of the buffer into the file starting start bytes in
 * returns number of bytes actually written
 */
uint32 ext2_write_file_by_inode( struct ext2_filesystem *fs,
                                 struct ext2_dir_entry_2 *file,
                                 void *buffer, uint32 start, uint32 nbytes ) {

    uint32 inode_num = file->inode;
    struct ext2_inode *fp = ext2_get_inode( fs, inode_num );
    uint32 blk_size = get_block_size( fs->sb );

    if ( fp->i_size < ( nbytes+start ) ) {
        if ( start > fp->i_size ) {
            printf( "Error: data not contiguous\n" );
            return 0;
        } else {
            increase_inode_size( fs, fp, nbytes+start-( fp->i_size ) );
        }
    }

    uint32 remaining_bytes = nbytes;
    // skip to the block to write into
    uint32 first_inode_blk = start / blk_size;
    uint32 direct_inode_blk_index;
    uint32 indir_inode_blk_index;
    if ( first_inode_blk >= EXT2_NDIR_BLOCKS )
    {
        indir_inode_blk_index = first_inode_blk - EXT2_NDIR_BLOCKS;
        direct_inode_blk_index = EXT2_NDIR_BLOCKS;
    }
    else {
        indir_inode_blk_index = 0;
        direct_inode_blk_index = first_inode_blk;
    }

    // start writing in the middle of a block
    uint32 blk_offset = start % blk_size;

    uint32 inode_blk_index;
    for( inode_blk_index = direct_inode_blk_index;
         inode_blk_index < EXT2_N_BLOCKS;
         inode_blk_index ++ )                       {

        uint32 blk_num = fp->i_block[inode_blk_index];

        char *data = (char *) ( blk_offset +
                              (void *)block_num_to_addr( fs, blk_num ));
        void *indir_blk;

        if ( inode_blk_index == EXT2_NDIR_BLOCKS ) {
            indir_blk = block_num_to_addr( fs, blk_num );
            if ( indir_inode_blk_index < indirect_data_block_size( fs ) )
                inode_blk_index --;
            data = block_num_to_addr( fs,
                            *( (uint32 *) indir_blk + indir_inode_blk_index ) );
            data += blk_offset;
            indir_inode_blk_index ++;
        }

        if ( remaining_bytes < blk_size ) {
            memcpy( data, buffer, remaining_bytes );
            remaining_bytes = 0;
            break;
        }
        else {
            memcpy( data, buffer, blk_size-blk_offset );
            buffer += ( blk_size-blk_offset );
            remaining_bytes -= ( blk_size-blk_offset );
        }
        blk_offset = 0;
    }
    return nbytes-remaining_bytes;
}

/*
 * Write nbytes from buffer into the file given by path starting start bytes in
 * Returns a write_status enum
 * Stores number of bytes successfully written in bytes_written
 */
ext2_write_status ext2_write_file_by_path( struct ext2_filesystem *fs,
                                           const char *path,
                                           const char *buffer,
                                           uint32 *bytes_written,
                                           uint32 start,
                                           uint32 nbytes )              {

    if ( !bytes_written ) {
        uint32 throwaway = 0;
        bytes_written = &throwaway;
    }
    *bytes_written = 0;

    if ( path[0] == DIR_SEP )
        path ++;

    uint path_len = strnlen( path, EXT2_NAME_LEN );
    char *filename = strrchr( path, DIR_SEP ) + 1;

    char dirname[path_len+1];
    memcpy( dirname, path, path_len+1 );
    ( (char*) strrchr( dirname, DIR_SEP ) )[1] = '\0';
    ( (char*) strrchr( dirname, DIR_SEP ) )[1] = '\0';

    struct ext2_dir_entry_2 *file = ext2_get_dirent_from_path( fs, dirname, filename );

    if ( file ) {
        *bytes_written = ext2_write_file_by_inode( fs, file, (void *)buffer, start, nbytes );
        return EXT2_WRITE_SUCCESS;
    }
    else {
        return EXT2_WRITE_FILE_NOT_FOUND;
    }

}
