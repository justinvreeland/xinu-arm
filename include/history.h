/*
 * @file history.h
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <shell.h>

#define HISTORY_LENGTH 10

typedef struct {
	char command[SHELL_BUFLEN]; // command itself
	uint commandLength;			// number of chars in the command
} history_item;

// stores the given command in the history
void addHistoryItem(char *command, uint numChars);

// print out all the history items in the form "Index: Command\n"
void printHistory();

#endif