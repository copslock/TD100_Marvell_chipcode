/***************************************************************************************** 
Copyright (c) 2009, Marvell International Ltd. 
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Marvell nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MARVELL ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MARVELL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************************/


#include "misc.h"

/* Get log file name from command line*/
int ParseLogFileName(char* pCmdLine,char* logfile)
{
    char *pCur, *pEnd;
    char par_name[1024];
    char *p1 = NULL, *p2 = NULL, *p3 = NULL;

    int  par_name_len         = 0;
    int  par_value_len        = 0;
  
    IppCodecStatus   ret      = IPP_STATUS_NOERR;
    char cTmp[64]             = {"\0"};

    pCur = pCmdLine;
    pEnd = pCmdLine + IPP_Strlen(pCmdLine);
    while((p1 = IPP_Strstr(pCur, "-"))){
        p2 = IPP_Strstr(p1, ":");
        if (NULL == p2) {
            return IPP_STATUS_ERR;
        }
        p3 = IPP_Strstr(p2, " "); /*one space*/
        if (NULL == p3) {
            p3 = pEnd;
        }

        par_name_len    = p2 - p1 - 1;
        par_value_len   = p3 - p2 - 1;

        if ((0 >= par_name_len)  || (1024 <= par_name_len) ||
            (0 >= par_value_len) || (1024 <= par_value_len)) {
                return IPP_STATUS_ERR;
        }

        IPP_Strncpy(par_name, p1 + 1, par_name_len);
         *(par_name+par_name_len) = '\0';
         if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
            IPP_Strncpy(logfile, p2 + 1, par_value_len);
              *(logfile+par_value_len) = '\0';
            return IPP_OK;
        } 
        pCur = p3;
    }

    return IPP_FAIL;
}

#ifdef _IPP_WINCE
#include <windows.h>

/* Convert string from unicode to asiic */
char *UnicodeToAnsi(LPCWSTR str)
{
    int iSize;
    char* pszMultiByte;

    iSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	IPP_MemMalloc(&pszMultiByte, iSize+1, 1);
	if(pszMultiByte != NULL){
        WideCharToMultiByte(CP_ACP, 0, str, -1, pszMultiByte, iSize, NULL, NULL);
        pszMultiByte[iSize]=0;
    }

    return pszMultiByte;
}

/* main entry for Windows Mobile OS */
int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0, retLog = 0;
    char** ppArgv;
    int index;
    void *p = NULL;
	char logFile[512];

    IPP_InitMemCheck();
    IPP_InitPerfCounter();

	IPP_MemMalloc(&ppArgv,argc*sizeof(char*),8);
    for(index = 0; index < argc; index++)
    {
        ppArgv[index] = UnicodeToAnsi(argv[index]);
    }

	if(argc>1)
	{
		retLog = ParseLogFileName(ppArgv[1],logFile);
	}
    
    ret = CodecTest(argc, ppArgv);    

    for(index = 0; index < argc; index++)
    {
		IPP_MemFree(&(ppArgv[index]));
    }
	IPP_MemFree(&ppArgv);

	IPP_DeinitMemCheck();
    IPP_DeinitPerfCounter();
	
	if(retLog == IPP_OK)
	{
		if(ret == IPP_OK)
		{
			IPP_Log(logFile,"a","Everything is OK!\n");
		}
		IPP_Log(logFile,"a","CodecTest finished!\n");
	}
	return 0;
}

#elif (defined _IPP_LINUX || defined _IPP_X86)

//main entry for Linux and x86win
int main(int argc, char* argv[])
{
	int ret = 0;
	char logFile[512];

    IPP_InitMemCheck();
    IPP_InitPerfCounter();

    ret = CodecTest(argc, argv);

    IPP_DeinitMemCheck();
    IPP_DeinitPerfCounter();
	
	if(argc>1 && ParseLogFileName(argv[1],logFile) == IPP_OK)
	{
		if(ret == IPP_OK)
		{
			IPP_Log(logFile,"a","Everything is OK!\n");
		}
		IPP_Log(logFile,"a","CodecTest finished!\n");
	}
	return 0;
}

#elif (defined _IPP_NUCLEUS)

#include"command.h"

//main entry for Nucleus OS
const int ippmain(int argc, char** argv)
{
	int ret = 0;
	char logFile[512];

    IPP_InitMemCheck();
    IPP_InitPerfCounter();

    ret = CodecTest(argc, argv);

    IPP_DeinitMemCheck();
    IPP_DeinitPerfCounter();
	
	if(argc>1 && ParseLogFileName(argv[1],logFile) == IPP_OK)
	{
		if(ret == IPP_OK)
		{
			IPP_Log(logFile,"a","Everything is OK!\n");
		}
		IPP_Log(logFile,"a","CodecTest finished!\n");
	}
	return 0;
}

__cmd(ippmain,"ippcodec","");
#else
//#error _IPP_WINCE,_IPP_LINUX,or _IPP_NUCLEUS must be defined
#endif

/* EOF */
