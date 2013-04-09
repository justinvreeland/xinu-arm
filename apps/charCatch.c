/**
 * @file charCatch.c
 * @provides charCatch
 *
 * Displays what characters where recieved when a key was pressed
 * and eventual will guess at the key.
 */

#include <conf.h>
#include <device.h>
#include <stdio.h>

int charCatch(void){
  int end;
  char a;

  end = 0;

  while(!end){

    a = getc(0);

    if( a == 27 ){
      printf("Char: ESC\tVal: %d\n", a);
    } else {
      printf("Char: %c\tVal: %d\n", a, a);
    }

    if( a== 4 ){
      end = 1;
    }
  }
  return( end );
}
