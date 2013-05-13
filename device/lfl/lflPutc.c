/*
 *
 */
#include <lfl.h>
#include <lfilesys.h>
#include "../apps/ext2_write.h"
#include "../apps/ext2_common.h"

devcall lflPutc(device *dev, char c){
  struct lflcblk *lfptr;

  lfptr = &lfltab[dev->minor];

  if( lfptr->lfstate != LF_USED ){
    return SYSERR;
  }

  printf("Pos %d\n", lfptr->lfpos);
  ext2_write_file_by_inode( Lf_data.xinufs, lfptr->dir, &c, lfptr->lfpos, 1);
  lfptr->lfpos++;
  return 0;

}
