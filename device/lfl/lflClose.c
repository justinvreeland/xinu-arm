/*
 *
 */
#include <lfl.h>
#include <lfilesys.h>
#include <ext2_common.h>

devcall lflClose(device* dev){
  struct lflcblk *lfptr;

  lfptr = &lfltab[dev->minor];

  lfptr->lfstate = LF_FREE;
  lfptr->lfdev = -1;
  lfptr->lfmode = 0;
  lfptr->lfpos = 0;
  lfptr->dir = 0;
  lfptr->fs = 0;
  lfptr->eof = 0;

  return OK;
}
