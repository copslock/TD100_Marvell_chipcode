/** @file MenuDisplay.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define CMDNAME_UNKNOWN		"UNKNOWN\n"
#define CMDNAME_LAST		"LASTCMD\n"
#define CMDDESC_BLANK		""
 
 

typedef struct  CmdMenu
{
	char       *pCmdName;
	char       *pCmdDescriptor;
} CmdMenu;
  
 
void DisplayMenu(CmdMenu *cmds, int cmdOption=-1);
