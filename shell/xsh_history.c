/*
 * @file     xsh_history.c
 * @provides xsh_history
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <history.h>
 #include <stdio.h>

shellcmd xsh_history(int nargs, char *args[])
{
	// char buf[5];
	// fgets(buf, 4, stdin);
	// buf[4] = '\0';
	// printf("It works! %s\n", buf);
	printHistory();
    return OK;
}
