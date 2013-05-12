/**
 * @file pipePutC.c
 * @provides pipePutC.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>

devcall pipePutC(device *devptr, char ch)
{
	pipe *pipe = &pipetab[devptr->minor];

	wait(pipe->emptyCount);

	pipe->buffer[pipe->writeLocation] = ch;
	pipe->writeLocation = (pipe->writeLocation + 1) % PIPE_SIZE; 
	
	signal(pipe->fullCount);
}