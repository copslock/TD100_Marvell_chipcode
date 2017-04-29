/** @file utilities_os.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "utilities_os.h"
#include "utilities.h"

off_t filelength(const char *filename)
{
    static	struct stat 	sbuf;
	int err=0; 

	err = stat(filename, &sbuf);
	if (err) return -errno;     //return a negative number as error code, 
	                            //otherwise it will be confused with a valid file length.

	return sbuf.st_size; 
}

int txtPtrCnt(int a) 
{
	return  (2*3 -1+LN_LINE_END) + (4*3 -1+LN_LINE_END) + a*3 + (a/(sizeof(DWORD)*4)*LN_LINE_END);
}

#ifdef _LINUX_
int getRestofFile(FILE *hFile, char** theFile)
{
	int len=0;
	char tempStr[MAX_LENGTH] = "";
	char *temp=0;
	char *tobeCat=0;
	fgets(tempStr, MAX_LENGTH, hFile);
	while (!feof(hFile))
	{
		if (!len)
		{
			len = strlen (tempStr);
			temp = (char*) malloc(len);
			if (!temp)
			{
				DebugLogRite("Cannot Allocate memory\n");
				return -1;
			}
			strcpy(temp,tempStr);
		}
		else
		{
			len += strlen(tempStr);
			temp = (char*) realloc(temp, len);
			if  (!temp)
			{
				DebugLogRite("Cannot realloc memory\n");
				return -1;
			}
			tobeCat = temp + len - strlen(tempStr);
			strncpy(tobeCat,tempStr,strlen(tempStr));
		}
		fgets(tempStr,MAX_LENGTH,hFile);
	}
	*theFile = temp;
	return 0;

}
int GetPrivateProfileSection(char *tag, char *key, int len, char *FileName)
{
	FILE *hFile = NULL;
	char sectionheader[MAX_LENGTH] = "";
	char tempString[MAX_LENGTH] = "";
	int err;
	char keytemp[MAX_LENGTH] = "";
	char value[MAX_LENGTH] = "";
	int retLen=0;

	strcpy(sectionheader,"["); 
	strcat(sectionheader,tag);
	strcat(sectionheader,"]"); 
	hFile = fopen(FileName, "r");	//for read only
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}
	else
	{
		fgets(tempString, MAX_LENGTH,hFile);
		// first find the section
		while (!feof(hFile)) 
		{
			if(strlen(sectionheader) <= strlen(tempString))
				if(!strncmp(tempString, sectionheader, strlen(sectionheader)))
					break; // found the section
			fgets(tempString,MAX_LENGTH,hFile);
		}

		//no section found
		if (feof(hFile))
		{
			err = fprintf(hFile, "%s\n", sectionheader);
		}
		else	
		{
			//section found
			//search for the next setion and count the length
			char *restoffile = 0;
			
			fgets(tempString, MAX_LENGTH,hFile);
			while (!feof(hFile)) // in the section
			{
				if('[' == tempString[0]) //  the start of next section 
				{
					break;
				}
				else
				{
					retLen += strlen(tempString);
				} 
				if (retLen >= len)
				{
					retLen = len;
					break;
				}
				else
				{
					fgets(tempString, MAX_LENGTH, hFile);
				}
			}
		}
		//close the file
		fclose (hFile);
	}

	return retLen;
}

int WritePrivateProfileString(char *tag, char *key, char *buf, char *FileName)
{
	FILE *hFile = NULL;
	char sectionheader[MAX_LENGTH] = "";
	char tempString[MAX_LENGTH] = "";
	int err;
	char keytemp[MAX_LENGTH] = "";
	char value[MAX_LENGTH] = "";

	strcpy(sectionheader,"["); 
	strcat(sectionheader,tag);
	strcat(sectionheader,"]"); 
	hFile = fopen(FileName, "a+");	//if the file doesn't exit create a new one
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}
	else
	{
		fclose(hFile);
	}

	hFile = fopen(FileName, "r+"); //open file for read and write
	if (NULL == hFile)
	{
		DebugLogRite("Cannot open file %s\n", FileName);
		return -1;
	}
	else
	{
		fgets(tempString, MAX_LENGTH,hFile);
		// first find the section
		while (!feof(hFile)) 
		{
			if(strlen(sectionheader) <= strlen(tempString))
			{
				if(!strncmp(tempString, sectionheader, strlen(sectionheader)))
				{
					break; // found the section
				}
			}
			fgets(tempString,MAX_LENGTH,hFile);
		}

		//no section found
		if (feof(hFile))
		{
			err = fprintf(hFile, "%s\n", sectionheader);
			err = fprintf(hFile, "%s = %s\n", key, buf);
		}
		else	//search for the key
		{
			//section found
			fpos_t curFilePos;
			char *restoffile = 0;
			//get the first line after the section
			err = fgetpos (hFile, &curFilePos);	
			if (err)
			{
				DebugLogRite("Fail to get current file position!%d\n",err);
				fclose (hFile);
				return -1;
			}
			fgets(tempString, MAX_LENGTH,hFile);
			while (!feof(hFile)) // in the section
			{
				if('[' == tempString[0]) //  the start of next section 
				{
					//Get the rest of the file content from we last mark the position
					err = fsetpos(hFile, &curFilePos);
					if (err)
					{
						DebugLogRite("Fail to set file position(%d)\n",err);
						fclose (hFile);
						return -1;
					}
					if (getRestofFile(hFile, &restoffile))
						return -1;
					//write the new key to the last position we made the mark
					if (fsetpos(hFile, &curFilePos))
					{
						DebugLogRite("Fail to set file position\n");
						fclose (hFile);
						return -1;
					}
					sprintf(tempString, "%s = %s\n", key, buf);
					fputs(tempString, hFile);
					//put the rest of the file back
					fputs(restoffile, hFile);
					free(restoffile);
					break;
				}
				else
				{
					sscanf(tempString, "%s = %s", keytemp, value); 
					if(!strncmp(keytemp, key, strlen(key)) )
					{
						//found the key, need to replace
						if (getRestofFile(hFile, &restoffile))
							return -1;
// DebugLogRite ("the rest of the file  %d\n",strlen(restoffile));
						//set the position back to the key found
						if (fsetpos(hFile, &curFilePos))
						{
							fclose(hFile);
							return -1;
						}
						sprintf(tempString, "%s = %s\n", key, buf);
						fputs(tempString, hFile);
						//put the rest of the file back.
						fputs(restoffile, hFile);
						free(restoffile);
						break;
					}
					else
					{
						//get the position, this could be the position of the key
						// or the position of the start of the next section
						if (fgetpos (hFile, &curFilePos))
						{	
							DebugLogRite("Fail to get current file position!\n");
							fclose (hFile);
							return -1;
						}
					}
				} 
				fgets(tempString, MAX_LENGTH, hFile);
			}

			if (feof(hFile))	//cannot find the key in the section
			{
				err = fprintf(hFile, "%s = %s\n", key, buf);
			}
		}
		//close the file
		fclose (hFile);
	}

	return strlen(buf);
}

int GetPrivateProfileString(char *tag, char *key, char *defValue, char *buf, 
							int bufLenInByte, char *FileName)
{
	FILE * hFile = NULL;
	char sectionheader[MAX_LENGTH]="", tempString[MAX_LENGTH]=""; 
	char keytemp[MAX_LENGTH]="", value[MAX_LENGTH]=""; 

	strcpy(buf, defValue); 

	strcpy(sectionheader,"["); 
	strcat(sectionheader,tag);
	strcat(sectionheader,"]"); 

	hFile = fopen(FileName, "r");
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}
	else
	{
		// first find the section
		while (!feof(hFile)) 
		{
 			fgets(tempString, MAX_LENGTH, hFile);
			if(strlen(sectionheader) <= strlen(tempString))
				if(!strncasecmp(tempString, sectionheader, strlen(sectionheader)))
				break; // found the section
		}
		while (!feof(hFile)) // in the section
		{
 			fgets(tempString, MAX_LENGTH, hFile);
			if('[' == tempString[0]) //  the start of next section no more hope
				break;
			else
			{
				sscanf(tempString, "%[^= ] = %s", keytemp, value); 
				
				if(!strcasecmp(keytemp, key)) 
				{
					strcpy(buf, value); 
					break;
				}
			} 
		}

		fclose(hFile);
	}

	return strlen(buf);
}

int GetPrivateProfileInt(char *tag,char *key , int defValue, 
								char *FileName)
{
	int		value=0, cnt; 
	char valueStr[MAX_LENGTH]=""; 

	value = defValue;

	cnt= GetPrivateProfileString( tag, key ,"", valueStr, MAX_LENGTH, 
								FileName);

	if(0 != cnt)
	{
		if('x' == valueStr[1] || 'X' == valueStr[1] ) 
			sscanf(&valueStr[2], "%x", &value); 
		else
			sscanf(valueStr, "%d", &value); 
	}

	return value;
 
}

int strnicmp(const char *str1, const char *str2, size_t count)
{
	return strncasecmp(str1, str2, count);
}

#endif	//_LINUX_

