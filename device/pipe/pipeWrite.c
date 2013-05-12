/**
 * @file pipeWrite.c
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>
 
devcall pipeWrite(device *devptr, char *buf, uint len)
{
	int i;
	for(i = 0; i < len; ++i) {
		pipePutC(devptr, buf[i]);
	}
}