/*
 * Second Extended Filesystem
 * File: ext2.h
 * Author: Catherine Sullivan
 * Based on O'Reilly Understanding the Linux Kernel Ext2 Filesystem
 */

#ifndef _EXT2_H
#define _EXT2_H

#define MEM_LOWER_BOUND 128
#define MEM_UPPER_BOUND 256

#define VIRT_MEM_LOCATION 256

// Magic number for ext2 filesystems
#define EXT2_MAGIC      0xEF53

// Structure sizes, in bytes
#define EXT2_SUPERBLOCK_SIZE                    1024
#define EXT2_BLOCK_GROUP_DESCRIPTOR_SIZE        32
#define EXT2_INODE_SIZE                         128
#define EXT2_NAME_LEN                           245

// Constants relative to the data blocks
#define EXT2_NDIR_BLOCKS        12
#define EXT2_NIND_BLOCKS        1
#define EXT2_N_BLOCKS           EXT2_NIND_BLOCKS + EXT2_NDIR_BLOCKS

// Important locations, relative to the start of the filesystem
#define EXT2_SUPERBLOCK_LOCATION        0x400

// Constants for state of FS
#define EXT2_VALID_FS   1       // Unmounted cleanly
#define EXT2_ERROR_FS   2       // Errors detected

// How to behave on an error
#define EXT2_ERRORS_CONTINUE    1       // Continue as if nothing happened
#define EXT2_ERROR_RO           2       // Remount read-only
#define EXT2_ERROR_PANIC        3       // Cause a kernel panic

// OS Creator values
#define EXT2_OS_LINUX   0       // Linux
#define EXT2_OS_HURD    1       // GNU HURD
#define EXT2_OS_MASIX   2       // MASIX
#define EXT2_OS_FREEBSD 3       // FreeBSD
#define EXT2_OS_LITES   4       // Lites
#define EXT2_OS_XINU    12      // Xinu

#define DIR_SEP '/'
#define EXT2_INODE_ROOT 1
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;
typedef          int    int32;
typedef          short  int16;
typedef          char   int8;

struct ext2_super_block {
    uint32      s_inodes_count;                 // Total number of inodes
    uint32      s_blocks_count;                 // Filesystem size in blocks
    uint32      s_r_blocks_count;               // Number of reserved blocks
    uint32      s_free_blocks_count;            // Free blocks counter
    uint32      s_free_inodes_count;            // Free inodes counter
    uint32      s_first_data_block;             // Num of 1st blk (always 1)
    uint32      s_log_block_size;               // Block size
    uint32      s_log_frag_size;                    // Fragment size
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
    uint16      s_def_resuid;                   // Default UID for reserved blks
    uint16      s_def_resgid;                   // Default GID for reserved blks
    uint32      s_first_ino;                    // Num of 1st nonreserved inode
    uint16      s_inode_size;                   // Size of on-disk inode struct
    uint16      s_block_group_nr;               // Blk grp num of this sblk
    uint32      s_feature_compat;               // Compatible feature bitmap
    uint32      s_feature_incompat;             // Incompatible features bitmap
    uint8       s_uuid[16];                     // 128-bit filesystem identifier
    char        s_volume_name[16];              // Volume name
    char        s_last_mounted[16];             // Pathname of last mount point
    uint32      s_algorithm_usage_bitmap;       // Used for compression
    uint8       s_prealloc_blocks;              // Num of blocks to preallocate
    uint8       s_prealloc_dir_blocks;          // Num of blks to preal for dirs
    uint16      s_padding1;                     // Alignment to word
    uint32      s_reserved[217];                // Nulls to pad out 1024 bytes
};

// Each block group has its own group descriptor
// Each bit in the block bitmap represents the current state of a block in
//      the block group. 1 means used, and 0 means free.
// The inode bitmap works the same way.
struct ext2_group_desc {
    uint32      bg_block_bitmap;        // Block number of block bitmap
    uint32      bg_inode_bitmap;        // Block number of inode bitmap
    uint32      bg_inode_table;         // Block number of first inode table blk
    uint16      bg_free_blocks_count;   // Number of free blocks in the group
    uint16      bg_free_inodes_count;   // Number of free indoes in the group
    uint16      bg_used_dirs_count;     // Number of directories in the group
    uint32      bg_reserved[3];         // Nulls to pad out 24 bytes
};

struct ext2_inode {
    uint16              i_mode;                 // File type and access rights
    uint16              i_uid;                  // Owner identifier
    uint32              i_size;                 // File length in bytes
    uint32              i_atime;                // Time of last file access
    uint32              i_ctime;                // Time that inode last changed
    uint32              i_mtime;                // Time file contents changed
    uint32              i_dtime;                // Time of file deletion
    uint16              i_gid;                  // Group identifier
    uint16              i_links_count;          // Hard links counter
    uint32              i_blocks;               // Num of data blks of the file
    uint32              i_flags;                // File flags
    union {
        struct {
            uint32      l_i_reserved2;
        }               linux1;
        struct {
            uint32      l_i_translator;
        }               hurd1;
        struct {
            uint32      m_i_reserved1;
        }               masix1;
    }                   osd1;                   // Specific OS info
    uint32              i_block[EXT2_N_BLOCKS]; // Pointers to data blocks
    uint32              i_generation;           // File vs. (for a network fs)
    uint32              i_file_acl;             // File access control list
    uint32              i_dir_acl;              // Directory access control list
    uint32              i_faddr;                // Fragment address
    union {
        struct {
            uint8       l_i_frag;               // Frag num (always 0)
            uint8       l_i_fsize;              // Frag size (always 0)
            uint16      i_padl;
            uint16      l_i_uid_high;           // High 16bit of user_id
            uint16      l_i_guid_high;          // High 16bit of group id
            uint32      l_i_reserved2;
        }               linux2;
        struct {
            uint8       h_i_frag;               // Frag num
            uint8       h_i_fsize;              // Frag size
            uint16      h_i_mode_high;          // High 16bit of 32bit mode
            uint16      h_i_uid_high;           // High 16bit of user id
            uint16      h_i_gid_high;           // High 16bit of group id
            uint32      h_i_author;             // user id of file author
        }               hurd2;
        struct {
            uint8       m_i_frag;               // Frag num (always 0)
            uint8       m_i_fsize;              // Frag size (always 0)
            uint16      m_pad1;
            uint32      m_i_reserved2[2];
        }               masix2;
    }                   osd2;                   // Specific OS info
};

struct ext2_dir_entry_2 {
    uint32                              inode;                 // Inode number
    struct ext2_dir_entry_2            *next_dirent;           // Next direntry
    uint8                               name_len;              // Filename length
    uint8                               filetype;              // File type
    char                                name[EXT2_NAME_LEN];   // Filename
};

// Defined i_mode values
typedef enum {
    EXT2_S_IFSOCK     = 0xC000,  // Socket
    EXT2_S_IFLNK      = 0xA000,  // Symbolic link
    EXT2_S_IFREG      = 0x8000,  // Regular file
    EXT2_S_IFBLK      = 0x6000,  // Block device
    EXT2_S_IFDIR      = 0x4000,  // Directory
    EXT2_S_IFCHR      = 0x2000,  // Character device
    EXT2_S_IFIFO      = 0x1000,  // Fifo
    EXT2_S_ISUID      = 0x0800,  // Set process user id
    EXT2_S_ISGID      = 0x0400,  // Set process group id
    EXT2_S_ISVTX      = 0x0200,  // Sticky bit
    EXT2_S_IRUSR      = 0x0100,  // User read
    EXT2_S_IWUSR      = 0x0080,  // User write
    EXT2_S_IXUSR      = 0x0040,  // User execute
    EXT2_S_IRGRP      = 0x0020,  // Group read
    EXT2_S_IWGRP      = 0x0010,  // Group write
    EXT2_S_IXGRP      = 0x0008,  // Group execture
    EXT2_S_IROTH      = 0x0004,  // Others read
    EXT2_S_IWOTH      = 0x0002,  // Others write
    EXT2_S_IXOTH      = 0x0001,  // Others execute
} i_mode;

// Different types of files recognized by ext2
typedef enum {
    EXT2_FT_UNKNOWN     = 0,    // Unknown
    EXT2_FT_REG_FILE    = 1,    // Regular file
    EXT2_FT_DIR         = 2,    // Directory
    EXT2_FT_CHRDEV      = 3,    // Character device
    EXT2_FT_BLKDEV      = 4,    // Block device
    EXT2_FT_FIFO        = 5,    // Named pipe
    EXT2_FT_SOCK        = 6,    // Socket
    EXT2_FT_SYMLINK     = 7,    // Symbolic link
    EXT2_FT_MAX
} file_type;

// How ext2 should behave when accessing the data for this inode
typedef enum {
    EXT2_SECRM_FL     =  0x00000001,      // Secure deletion
    EXT2_UNRM_FL      =  0x00000002,      // Record for undelete
    EXT2_COMPR_FL     =  0x00000004,      // Compressed file
    EXT2_SYNC_FL      =  0x00000008,      // Synchronous updates
    EXT2_IMMUTABLE_FL =  0x00000010,      // Immutable file
    EXT2_APPEND_FL    =  0x00000020,      // Append only
    EXT2_NODUMP_FL    =  0x00000040,      // Do not dump/delete file
    EXT2_NOATIME_FL   =  0x00000080,      // Do not update i_atime
    EXT2_DIRTY_FL     =  0x00000100,      // Dirty (modified)
    EXT2_COMPRBLK_FL  =  0x00000200,      // Compressed block
    EXT2_NOCOMPR_FL   =  0x00000400,      // Access raw compressed data
    EXT2_ECOMPR_FL    =  0x00000800,      // Compression error
    EXT2_BTREE_FL     =  0x00001000,      // B-tree format directory
    EXT2_INDEX_FL     =  0x00002000,      // Hash indexed directory
    EXT2_IMAGIC_FL    =  0x00004000,      // AFS directory
    EXT2_RESERVED_FL  =  0x80000000,      // Reserved for ext2 library
} inode_flags;

struct ext2_filesystem {
    struct ext2_super_block *sb;         // Superblock for the fs
    uint32 base_addr;                   // Base address of the fs
};

#endif // _EXT2_H
