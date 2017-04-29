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

/******************************************************************************
//  Description:    encoder parameter process functions
//	Function list:
//		parSkipSpace
//		parSkipComment
//		parGetToken
//
******************************************************************************/

#include "codecVC.h"
#include "vguser.h"
/******************************************************************************
//  parSkipSpace	
//  parSkipComment  
//  parGetToken		
******************************************************************************/


/***************************************************************************
// Name:             parSkipSpace
// Description:      Skip the space in the input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parSkipSpace(FILE *m_fp)
{
	int ch;
	do {
		ch = getc(m_fp);
		if(ch==EOF)
			return ERES_EOF;
	} while(isspace(ch));

	ungetc(ch, m_fp);
	return ERES_OK;

}

/***************************************************************************
// Name:             parSkipComment
// Description:      Skip the comments in the input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parSkipComment(FILE *m_fp)		
{
	int ch;
	do {
		ch = getc(m_fp);
		if(ch==EOF)
			return ERES_EOF;

		if(ch!='/' && ch!='*')
		{
			ungetc(ch, m_fp);
			return ERES_NOOBJ;
		}

		if(ch=='/')
		{
			do {
				ch = getc(m_fp);
				if(ch==EOF)
					return ERES_EOF;
			} while(ch!='\n');
		}
		else if(ch=='*')
		{
			int iState = 0;			
			do {
				ch = getc(m_fp);
				if(ch==EOF)
					return ERES_EOF;
				if(iState==0 && ch=='*')
					iState = 1;
				else if(iState==1)
				{
					if(ch=='/')
						iState = 2;
					else if(ch!='*')
						iState = 0;
				}
			} while(iState!=2);
		}
		if(parSkipSpace(m_fp)==ERES_EOF) {
			return ERES_EOF;
		}
		ch = getc(m_fp);
		if(ch==EOF) {
			return ERES_EOF;
		}

	} while(ch=='/');
	
	ungetc(ch, m_fp);
	return ERES_OK;
}

/***************************************************************************
// Name:             parGetToken
// Description:      Get token to output buffer from input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
//		pchBuf		 Pointer to the output char buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parGetToken(FILE *m_fp, char *pchBuf)   
{
	int ch;	
	int i=1,j=0;
	parSkipSpace(m_fp);
	ch = getc(m_fp);
	if(ch==EOF) {
		return ERES_EOF;
	}

	if(ch=='/')	{
		parSkipComment(m_fp);
		ch = getc(m_fp);
		if (ch==EOF)
			return ERES_EOF;
	} 
	if(ch=='=')
	{	
		pchBuf[0] = (char)ch;
		pchBuf[1] = '\0';
		return ERES_OK;
	}

	pchBuf[0] = (char)ch;

	if(ch=='\"')	{	
		ch = getc(m_fp);
		while (ch != '\"'){
			pchBuf[j] = (char)ch;
			ch = getc(m_fp);
			j++;
		}
		pchBuf[j] = '\0';
		return ERES_OK;
	} else{
		for(i=1;i<TOKEN_SIZE - 1;i++)
		{
			ch = getc(m_fp);
			if(ch==EOF) {
				pchBuf[i] = '\0';
				return ERES_EOF;
			}
			if(isspace(ch)||ch == '=')
			{
				pchBuf[i] = '\0';
				ungetc(ch, m_fp);
				return ERES_OK;
			}

			if(ch=='/' && parSkipComment(m_fp)!=ERES_NOOBJ)
			{
				pchBuf[i] = '\0';
				return ERES_OK;
			}
			pchBuf[i] = (char)ch;
		}
	}
	pchBuf[i] = '\0';
	return ERES_OK;

}





