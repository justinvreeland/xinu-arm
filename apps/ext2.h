/*
 * Second Extended Filesystem
 * Author: Catherine Sullivan
 * Based on O'Reilly Understanding the Linux Kernel Ext2 Filesystem
 */

#ifndef _EXT2_H
#define _EXT2_H

#include "types.h"

// Magic number for ext2 filesystems
#define EXT2_MAGIC      0xEF53

// Structure sizes, in bytes
#define EXT2_SUPERBLOCK_SIZE                    1024
#define EXT2_BLOCK_GROUP_DESCRIPTOR_SIZE        32
#define EXT2_INODE_SIZE                         128
#define EXT2_NAME_LEN                           255

// Constants relative to the data blocks
#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK        (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK        (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS          (EXT2_TIND_BLOCK + 1)

// Important locations, relative to the start of the filesystem
#define EXT2_SUPERBLOCK_LOCATION        0x400


struct ext2_superblock {
    uint32sx_inodes_count;                      // Total number of inodes
    uint32      s_blocks_count;                 // Filesystem size in blocks
    uint32      s_r_blocks_count;               // Number of reserved blocks
    uint32      s_free_blocks_count;            // Free blocks counter
    uint32      s_free_inodes_count;            // Free inodes counter
    uint32      s_first_data_block;             // Num of 1st blk (always 1)
    uint32      s_log_block_size;               // Block size
     int32      s_frag_size;                    // Fragment size
    uint32      s_blocks_per_group;             // Number of blocks per group
    uint32      s_frags_per_group;              // Number of fragments per group
    uint32      s_inodes_per_group;             // Number of inodes per group
    uint32      s_mtime;                        // Time of last mount operation
    uint32      s_wtime;                        // Time of last write operation
    uint16      s_mnt_count;                    // Mout operations counter
    uint16      s_max_mnt_count;                // Num of mount ops before check
    uint16      s_magic;                        // Magic signature
    uint16      s_state;                        // Status flag
    uint16      s_errors;                       // Behavior when detecting errs
    uint16      s_minor_rev_level;              // Minor revision level
    uint32      s_lastcheck;                    // Time of last check
    uint32      s_checkinterbal;                // Time between checks
    uint32      s_creator_os;                   // OS where fs was created
    uint32      s_rev_level;                    // Revision level
    uint16      s_def_resuid                    // Default UID for reserved blks
    uint16      s_def_resgid;                   // Default GID for reserved blks
    uint32      s_first_ino;                    // Num of 1st nonreserved inode
    uint16      s_inode_size;                   // Size of on-disk inode struct
    uint16      s_block_group_nr;               // Blk grp num of this sblk
    uint32      s_feature_compat;               // Compatible feature bitmap
    uint32      s_feature_incompat;             // Incompatible features bitmap
    uint8 [16]  s_uuid;                         // 128-bit filesystem identifier
    char  [16]  s_volume_name;                  // Volume name
    char  [64]  s_last_mounted;                 // Pathname of last mount point
    uint32      s_algorithm_usage_bitmap;       // Used for compression
    uint8       s_prealloc_blocks;              // Num of blocks to preallocate
    uint8       s_prealloc_dir_blocks;          // Num of blks to preal for dirs
    uint16      s_padding1;                     // Alignment to word
    uint32[204] s_reserved;                     // Nulls to pad out 1024 bytes
};


// Each block group has its own group descriptor
struct ext2_group_desc {
    uint32      bg_block_bitmap;        // Block number of block bitmap
    uint32      bg_inode_bitmap;        // Block number of inode bitmap
    uint32      bg_inode_table;         // Block number of first inode table blk
    uint16      bg_free_blocks_count;   // Number of free blocks in the group
    uint16      bg_free_indoes_count;   // Number of free indoes in the group
    uint16      bg_used_dirs_count;     // Number of directories in the group
    uint32[3]   bg_reserved;            // Nulls to pad out 24 bytes
};

struct ext2_inode_struct {
    uint16                      i_mode;         // File type and access rights
    uint16                      i_uid;          // Owner identifier
    uint32                      i_size;         // File length in bytes
    uint32                      i_atime;        // Time of last file access
    uint32                      i_ctime;        // Time that inode last changed
    uint32                      i_mtime;        // Time file contents changed
    uint16                      i_gid;          // Group identifier
    uint16                      i_links_count;  // Hard links counter
    uint32                      i_blocks;       // Num of data blks of the file
    uint32                      i_flags;        // File flags
    union {
        struct {
            uint32              l_i_reserved2;
        }                       linux1;
        struct {
            uint32              l_i_translator;
        }                       hurd1;
        struct {
            uint32              m_i_reserved1;
        }                       masix1;
    }                           osd1;           // Specific OS info
    uint32[EXT2_n_BLOCKS]       i_block;        // Pointers to data blocks
    uint32                      i_generation;   // File vs. (for a network fs)
    uint32                      i_file_acl;     // File access control list
    uint32                      i_dir_acl;      // Directory access control list
    uint32                      i_faddr;        // Fragment address
    union {
        struct {
            uint8               l_i_frag;       // Fragment number
            uint8               l_i_fsize;      // Fragment size
            uint16              i_padl;
            uint16              l_i_uid_high;   // These 2 fields
            uint16              l_i_guid_high;  // were reserved2[0]
            uint32              l_i_reserved2;
        }                       linux2;
        struct {
            uint8               h_i_frag;       // Fragment number
            uint8               h_i_fsize;      // Fragment size
            uint16              h_i_mode_high;
            uint16              h_i_uid_high;
            uint16              h_i_gid_high;
            uint32              h_i_author;
        }                       hurd2;
        struct {
            uint8               m_i_frag;       // Fragment number
            uint8               m_i_fsize;
            uint16              m_pad1;
            uint32              m_i_reserved2[2];
        }                       masix2;
    }                           osd2;          // Specific OS info
};

// Different types of files recognized by ext2
typedef enum {
    EXT2_FT_UNKNOWN     = 0,    // Unknown
    EXT2_FT_REG_FILE    = 1,    // Regular file
    EXT2_FT_DIR         = 2,    // Directory
    EXT2_FT_CHRDEV      = 3,    // Character device
    EXT2_FT_BLKDEV      = 4,    // Block device
    EXT2_FT_FIFO        = 5,    // Named pipe
    EXT2_FT_SOCK        = 6,    // Socket
    EXT2_FT_SYMLINK     = 7     // Symbolic link
    EXT2_FT_MAX
};

struct ext2_dir_entry_2 {
    uint32              inode;          // Inode number
    uint16              rec_len;        // Directory entry length
    uint8               name_len;       // Filename length
    uint8               filetype;       // File type
    char[EXT2_NAME_LEN] name;           // Filename
}

#endif // _EXT2_H
