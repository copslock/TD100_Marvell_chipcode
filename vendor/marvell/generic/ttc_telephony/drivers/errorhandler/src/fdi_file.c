/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: fdi_file.c
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
#include "fdi_file.h"


FILE *FDI_fopen(const char *fName, char *openFlags)
{
	FILE *fdr;
	char fullPath[MAX_PATH] = { 0 };
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");

	if (nvm_root_dir
#if defined(OSA_LINUX)
			&& fName[0] != '/' /* if the first letter is '/', we mean absolute path */
#endif
			)
	{
		strcpy(fullPath, nvm_root_dir);
		strcat(fullPath, "/");
	}
	strcat(fullPath, fName);

	fdr = fopen(fullPath, openFlags);

	return fdr;
}

int FDI_fread(void *buffer, size_t size, size_t count, FILE* stream)
{
	return fread(buffer, size, count, stream);
}

int FDI_fwrite(void *buffer, size_t size, size_t count, FILE* stream)
{
	return fwrite(buffer, size, count, stream);
}

int FDI_fseek(FILE *stream, long offset, int position )
{
	return fseek(stream, offset, position);
}

int FDI_fclose(FILE* stream)
{
	return fclose(stream);
}

int  FDI_remove(void* fileName)
{
	char fullPath[MAX_PATH] = { 0 };
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");

	if (nvm_root_dir
#if defined(OSA_LINUX)
			&& ((char*)fileName)[0] != '/' /* if the first letter is '/', we mean absolute path */
#endif
			)
	{
		strcpy(fullPath, nvm_root_dir);
		strcat(fullPath, "/");
	}
	strcat(fullPath, fileName);

	return unlink(fullPath);
}

