/*
 * @file     xsh_echo.c
 * @provides xsh_echo
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>

/*
 * Shell command (echo).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_cp(int nargs, char *args[])
{
  cp( args[1], args[2] );
  args[1] = 0;
    return OK;
}
