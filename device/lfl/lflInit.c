/*
 * File: lflInit.c
 *
 * @author: Justin Vreeland
 *
 * Description: Initializes file psuedo device for xinu
 *
 * A large portion of this code is based off of the code found in:
 * Operating Systems, The Xinu Approach Linksys Version
 */

#include <lfl.h>
#include <lfs.h>
#include <lfilesys.h>

struct lflcblk lfltab[Nlfl];

devcall lflInit( struct dentry *devptr ){

  struct lflcblk *lfptr;
  int32 i;

  lfptr = &lfltab[ devptr->minor ];

  /* Initialize control block entry */
  lfptr->lfdev = devptr->num;
  lfptr->lfstate = LF_FREE;
  lfptr->lfdev = devptr->num;
  lfptr->lfpos = 0;

  return OK;
}
