/*
 * @file     xsh_echo.c
 * @provides xsh_echo
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
/*
 * Shell command (echo).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_ls(int nargs, char *args[])
{

  // sketchyest hack eva'
  printf("");
  if(!args[1]){
    *args[1] = '.';
  }
  char tmp[strnlen(args[1]) + 1];
  memcpy(tmp, args[1], strnlen(args[1]) + 1);
  tmp[strnlen(args[1])] = '/';
  tmp[strnlen(args[1]) + 1] = 0;

  ls(tmp);
  args[1] = 0;
    return OK;
}
