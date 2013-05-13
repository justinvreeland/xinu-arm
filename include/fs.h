/**
 * @file fs.h
 *
 * @author Justin Vreeland
 */


#define FILE_BLEN           1500  /* file buffer lenght */

/*
 * fs control block
 *
 * not entirely sure what should be here so i'm kind of copying tty
 */

struct fs_control{

  device *fs;               /* fs location */

  bool  ieof;               /* EOF read */
  bool ielim;               /* partial line in buffer */
  char in[FILE_BLEN];       /* input buffer */
  char out[FILE_BLEN];      /* output buffer */
  uint istart;              /* position of first char */
  uint icount;              /* number of chars in buffer */

};

extern struct fs fstab[];

/* driver functions */
devcall ext2Init(device *);
dkvcall ext2Close(device *);
devcall ext2Getc(device *, char);
devcall ext2Putc(device *, char);
devcall ext2Write(device *, void *, uint);
devcall ext2Read(device *, void *, uint);
devcall ext2Seek();

