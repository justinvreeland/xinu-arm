/*
 * @file     xsh_echo.c
 * @provides xsh_echo
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <ext2cheating.h>

/*
 * Shell command (echo).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_pwd(int nargs, char *args[])
{
  printf("%s\n", curdir);
}
