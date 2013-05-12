/**
 * @file pipeOpen.c
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>

devcall pipeOpen(device *devptr)
{
	pipe *pipe = &pipetab[devptr->minor];

	if(pipe->pipeState == PIPE_FREE) {
		pipe->pipeState = PIPE_USED;
	}
}