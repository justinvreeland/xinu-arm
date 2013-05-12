/*
 * @file     xsh_upper.c
 * @provides xsh_upper
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <upper.h>
#include <stdio.h>
#include <device.h>

shellcmd xsh_upper(int nargs, char *args[])
{
	char buf[256];
	read(stdin, buf, 256);
	printf("%s\n", uppercaseString(buf, 256));
    return OK;
}
