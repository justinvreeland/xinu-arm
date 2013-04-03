/**
 * @file history.c
 * @provides history
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <history.h>
#include <string.h>
#include <stdio.h>

int numHistoryItems = 0;		// number of history items currently stored
int currentHistoryIndex = 0;	// index into the history for when numHistoryItems
								// is less than HISTORY_LENGTH

history_item history[HISTORY_LENGTH]; // actual storage for the history

// shift all the history items back one item
void shiftHistoryItems()
{
	int i;
	for(i = 1; i < numHistoryItems; ++i) {
		history[i - 1] = history[i];
	}
}

void addHistoryItem(char *command, uint numChars)
{
	// create and init the history_item
	history_item item;
	item.commandLength = numChars;
	strncpy(item.command, command, SHELL_BUFLEN);

	// history is filled, start overwriting/shifting
	// to maintain "most recent" ordering
	if(numHistoryItems == HISTORY_LENGTH) {

		shiftHistoryItems();
		// newest item will always be at the end
		history[HISTORY_LENGTH - 1] = item;

	// otherwise, just store it and move the index
	} else {
		history[currentHistoryIndex++] = item;
	}
	
	// cap numHistoryItems at HISTORY_LENGTH
	if(numHistoryItems < HISTORY_LENGTH)
	{
		++numHistoryItems;
	}
}

void printHistory()
{
	int i;
	for(i = 0; i < numHistoryItems; ++i) {
		printf("%d: %s\n", i+1, history[i].command);
	}
}