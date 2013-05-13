/*
 *
 */

#include <lfl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

devcall lflSeek(void * useless){
  printf("lflSeek reached\n");

  return SYSERR;
}
