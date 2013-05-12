/**
 * @file pipeGetC.c
 * @provides pipeGetC.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>

devcall pipeGetC(device *devptr)
{
	char c;
	pipe *pipe = &pipetab[devptr->minor];

	wait(pipe->fullCount);

	c = pipe->buffer[pipe->readLocation];
	pipe->readLocation = (pipe->readLocation + 1) % PIPE_SIZE; 
	
	signal(pipe->emptyCount);
	
	return c;
}