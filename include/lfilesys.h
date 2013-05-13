/*  THIS IS MODELED AFTER BUT NOT IDENTICLE TO THE ONE IN THE XINUE BOOK BE WARRNEEENENNENE! */

#ifndef Nlfl
#define Nlfl 2   // no idea what that does
#endif

#ifndef LF_DISK_DEV  // more cluelessness
#define LF_DISK_DEV SYSERR
#endif

// I don't know if F_MOD_R is defined anywhere
#define LF_MODE_R   F_MODE_R      /* mode bit for "read" */
#define LF_MODE_W   F_MODE_W      /* mode bit for "write" */
#define LF_MODE_RW  F_MODE_RW     /* mode bit for "read or write */
#define LF_MODE_O   F_MODE_O      /* mode for "old" */
#define LF_MODE_N   F_MODE_N      /* mode bit for "new" */

// in the book here are settings I don't think we care about

#define LF_NAME_LEN 255
#define LF_FREE     0
#define LF_USED    1

#ifndef _LFILESYS_H_
#define _LFILESYS_H_

#include <ext2.h>

// in the book here are settings I don't think we care about
// i didn't set up mutexs becuase i don't care at the moment

struct lfdata {
  int lf_dev;                               /* device if to use */
  struct ext2_filesystem* xinufs;             /* pointer to mem location of fs */
};

struct lflcblk {
  char      lfstate;                          /* is entry free or used */
  int       lfdev;                            /* device id of this device */
  int       lfmode;                           /* mode (read/write/both) */
  uint32    lfpos;                            /* byte position of next byte to read or write */
  struct ext2_dir_entry_2* dir;               /* dir entry */
  struct ext2_filesystem* fs;                 /* filesystem pointer */
  bool      eof;

};

extern struct lfdata Lf_data;
extern struct lflcblk lfltab[];

#endif
