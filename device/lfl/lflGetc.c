/*
 * File: lflGetc.c
 *
 * Author: Justin Vreeland
 *
 * Description: provides the ablity to get characters from the filesystem
 *
 * based heavily off of code found in
 * Operating Systems the Xinu Approach Linksys Version
 * Things not implemented and probbly not going to be:
 *  1) flags
 *  2) exlusive acess
 *
 */

#include <lfl.h>
#include <lfs.h>
#include <lfilesys.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2_write.h>


devcall lflGetc(device* dev){
  struct lflcblk *lfptr;

  char c;

  lfptr = &lfltab[dev->minor];

  if( lfptr->lfstate != LF_USED ){
    return SYSERR;
  }

  if( !ext2_read_dirent( Lf_data.xinufs, lfptr->dir, &c, lfptr->lfpos, 1) ){
    lfptr->eof = 1;
    return SYSERR;
  }

  if( !c ){
    lfptr->eof = 1;
    return 0;
  }

  lfptr->lfpos++;
  return c;
}
