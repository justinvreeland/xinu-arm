/*
 * @file     xsh_history.c
 * @provides xsh_history
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <history.h>

shellcmd xsh_history(int nargs, char *args[])
{
	printHistory();
    return OK;
}
