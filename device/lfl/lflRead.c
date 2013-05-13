/*
 * Filename: lflRead.c
 *
 * Author: Justin Vreeland
 *
 * Description: Provides read function for local filessytem
 * pretty much identical to that found in
 *
 * Operating Systems The Xinu Approach Linksys Version
 */

#include <lfl.h>
#include <lfilesys.h>

devcall lflRead(device* dev, void* buff, uint count){

  int i;
  char c;

  if( count < 0){
    return SYSERR;
  }

  // ask what happens if i request more infoz
  for( i = 0; i < count; i++){
    c = lflGetc(dev);
    if(!c){
      return i;
    } else {
      ((char*)buff)[i] = c;
    }
  }

  return i;
}
