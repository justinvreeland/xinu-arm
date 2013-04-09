/**
 * @file history.c
 * @provides history
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <history.h>
#include <string.h>
#include <stdio.h>

int numHistoryItems = 0;	// number of history items currently stored
history_item history[HISTORY_LENGTH]; // actual storage for the history

// shift all the history items back one item
void shiftHistoryItems()
{
	int i;
	for(i = numHistoryItems - 1; i >= 0; --i) {
		if(i + 1 < HISTORY_LENGTH) {
			history[i + 1] = history[i];
		}
	}
}

void addHistoryItem(char *command, uint numChars)
{
	// create and init the history_item
	history_item item;
	item.commandLength = numChars;
	strncpy(item.command, command, SHELL_BUFLEN);

	shiftHistoryItems();
	history[0] = item;
	
	// cap numHistoryItems at HISTORY_LENGTH
	if(numHistoryItems < HISTORY_LENGTH)
	{
		++numHistoryItems;
	}
}

void printHistory()
{
	int i, count;
	for(i = numHistoryItems - 1, count = 1; i >= 0; --i, ++count) {
		printf("%d: %s", count, history[i].command);
	}
}
