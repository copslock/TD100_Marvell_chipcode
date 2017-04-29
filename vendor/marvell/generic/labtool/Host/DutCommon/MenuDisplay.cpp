/** @file MenuDisplay.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "MenuDisplay.h"
#include "utilities.h"

   
void DisplayMenu(CmdMenu *cmds, int cmdOption) 
{
	if(0 > cmdOption)
	{	
	// all
		int i=0;

		while (strnicmp(cmds[i].pCmdName, "LASTCMD", 7))
		{
			if (strnicmp(cmds[i].pCmdName, "UNKNOWN", 7))
			{
				if (0!=i)
					DebugLogRite("%4d : ", i);
				DebugLogRite(cmds[i].pCmdName);

				if (-2 == cmdOption)
				{
					if (strnicmp(cmds[i].pCmdName, "UNKNOWN", 7) && 
					0 != strlen(cmds[i].pCmdDescriptor))
						DebugLogRite(cmds[i].pCmdDescriptor);
				}
 			
			}
			i++;
		}
	}
	else
	{
	// one
		DebugLogRite(cmds[cmdOption].pCmdName);
		if (strnicmp(cmds[cmdOption].pCmdName, "UNKNOWN", 7) && 
			0 != strlen(cmds[cmdOption].pCmdDescriptor))
			DebugLogRite(cmds[cmdOption].pCmdDescriptor);
	}

}

