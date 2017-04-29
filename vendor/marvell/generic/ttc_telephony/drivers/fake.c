/*
 * fake.c
 * Wrappers for functions not provided by Bionic C Library
 *
 * (C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <string.h>

int init_stdfiles(void)
{
	FILE *fp;

	fp = fdopen(0, "r");    // STDIN
	if (fp) memcpy(&__sF[0], fp, sizeof(FILE));
	fp = fdopen(1, "w+");   // STOUT
	if (fp) memcpy(&__sF[1], fp, sizeof(FILE));
	fp = fdopen(2, "w+");   // STERR
	if (fp) memcpy(&__sF[2], fp, sizeof(FILE));

	return 0;
}

int openpty()
{
	return 1;
}
