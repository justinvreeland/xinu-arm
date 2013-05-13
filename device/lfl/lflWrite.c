/*
 * Filename: lflWrite.c
 *
 * Author: Justin Vreeland
 *
 * Description: provides function for local filesystem
 * writing almost identicle to code found
 *
 * Operating Systems The Xinu Approach Linksys Version
 */

#include <lfl.h>
#include <lfilesys.h>

devcall lflWrite(device* dev, void* buf, uint num){

  int i;
  int c;

  if( num < 0 ){
    return SYSERR;
  }

  for( i = 0; i < num; i++){
    c = lflPutc(dev, ((char *)buf)[i]);
  }

  return i;
}
