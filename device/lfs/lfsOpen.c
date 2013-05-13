/*
 *  File: lfsOpen.c
 *
 *  Author: Justin Vreeland
 *
 *  Description: Opens file and allocates local file psuedo device
 *
 *  This code is based off of code found in:
 *  Operating Systems, The Xinu Approach Linksys Version
 */

#include "ext2.h"
#include <lfs.h>
#include <lfl.h>
#include <lfilesys.h>
#include <string.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2_write.h>

/*
 * This this doesn't do and i have no intention of adding
 *  1) flags
 *  2) checking if the file is already open
 *  3) open file under different fs
 *      a) files have acess to lfs infoz they should have an fs field
 */
devcall lfsOpen( struct dentry* devptr, char* path, char* mode){

  printf("path: %s\n", path);

  struct lflcblk* lfptr;                  /* pointer to open file table entry */
  struct ext2_dir_entry_2* newEnt;        /* pointer to new entry */
  int lfnext;                           /* minor number of next dev */
  int  i;                               /* index */
  // int  mbits;                           /* mode bits */
  int  lastDir;                         /* location of last '/' */
  char* filename;
  char* fpath;

  for ( i = 0; i < Nlfl; i++){
    lfptr = &lfltab[i];
    if(lfptr->lfstate == LF_FREE){
      break;
    }
  }

  if( i == Nlfl )
    return SYSERR;

  filename = path;

  while( 1 ){
    fpath = filename;
    filename = strchr( filename, '/' );
    if( !filename )
      break;

    filename++;
  }

  int len = strnlen( fpath, 1500 );

  if(  len > LF_NAME_LEN )
    return SYSERR;

  char name[len];
  strncpy( name, fpath, len );
  char tmp = *fpath;
  *fpath = '\0';
  // without this i gain an extra '/'
  name[len] = 0;
  printf("FS: %x, Path: %s, Name: %s\n", Lf_data.xinufs, path, name);
  lfptr->dir = ext2_get_dirent_from_path( Lf_data.xinufs, path, name);
  print_dirent(lfptr->dir);
  lfptr->lfstate = LF_USED;
  print_dirent( lfptr->dir);
  // restore the string to what the caller expects
  *fpath = tmp;

  return  lfptr->lfdev;
}
