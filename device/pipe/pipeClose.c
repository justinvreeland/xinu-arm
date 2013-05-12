/**
 * @file pipeClose.c
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>

devcall pipeClose(device *devptr)
{
	pipe *pipe = &pipetab[devptr->minor];

	// reset all the pipe's properties
	pipe->pipeState = PIPE_FREE;

	semfree(pipe->emptyCount);
	pipe->emptyCount = semcreate(PIPE_SIZE);

	semfree(pipe->fullCount);	
	pipe->fullCount = semcreate(0);

	memset(pipe->buffer, '\0', PIPE_SIZE);
	
	pipe->writeLocation = 0;
	pipe->readLocation = 0;
}