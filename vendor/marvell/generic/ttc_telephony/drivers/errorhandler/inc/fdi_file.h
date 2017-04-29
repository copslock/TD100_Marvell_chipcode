/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: fdi_file.h
 *
 *  Description: The APIs to operate files.
 *
 *  History:
 *   March, 2009 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PATH                        256
#define FILE_ID  FILE*

extern FILE *FDI_fopen(const char *fName, char *openFlags);

extern int FDI_fread(void *buffer, size_t size, size_t count, FILE* stream);

extern int FDI_fwrite(void *buffer, size_t size, size_t count, FILE* stream);

extern int FDI_fseek(FILE *stream, long offset, int position );

extern int FDI_fclose(FILE* stream);

extern int  FDI_remove(void* fileName);

