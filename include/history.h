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

extern int numHistoryItems;	// number of history items currently stored
extern history_item history[HISTORY_LENGTH]; // actual storage for the history

// stores the given command in the history
void addHistoryItem(char *command, uint numChars);

// print out all the history items in the form "Index: Command\n"
void printHistory(void);

#endif