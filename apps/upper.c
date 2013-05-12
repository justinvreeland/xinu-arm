/**
 * @file upper.c
 * @provides upper
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <upper.h>
#include <ctype.h>

char * uppercaseString(char *string, int numChars)
{
	int i;
	for(i = 0; i < numChars; ++i) {
		if(!isupper(string[i])) {
			string[i] = toupper(string[i]);
		}
	}

	return string;
}