/**
 * @file pipe.h
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _PIPE_H_
#define _PIPE_H_

#include <semaphore.h>
#include <string.h>

#define NUM_PIPES		1

#define PIPE_FREE		0
#define PIPE_USED 		1
#define PIPE_CONNECTED 	2

#define PIPE_SIZE 		256

typedef struct {
	short 		pipeState;
	semaphore 	emptyCount;
	semaphore 	fullCount;
	char 		buffer[PIPE_SIZE];
	int 		writeLocation;
	int 		readLocation;
} pipe;

extern pipe pipetab[NUM_PIPES];

devcall pipeInit(device *devptr);
devcall pipeOpen(device *devptr);
devcall pipeClose(device *devptr);
devcall pipeWrite(device *devptr, char *buf, uint len);
devcall pipeRead(device *devptr, char *buf, uint len);
devcall pipeGetC(device *devptr);
devcall pipePutC(device *devptr, char ch);

#endif