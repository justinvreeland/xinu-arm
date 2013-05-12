/**
 * @file pipeInit.c
 * @provides pipeInit.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <pipe.h>

pipe pipetab[NUM_PIPES];

devcall pipeInit(device *devptr)
{
	pipe newPipe;
    pipetab[devptr->minor] = newPipe;

    pipe *pipe = &pipetab[devptr->minor];

    bzero(pipe, sizeof(pipe));

    pipe->pipeState = PIPE_FREE;
    pipe->emptyCount = semcreate(PIPE_SIZE);
    pipe->fullCount = semcreate(0);
    memset(pipe->buffer, '\0', PIPE_SIZE);
    pipe->writeLocation = 0;
    pipe->readLocation = 0;
}