/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
/***************************************************************************************** 
 *   Copyright (c) 2009, Marvell International Ltd. 
 *     All Rights Reserved.
 ****************************************************************************************/

/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#define LOG_TAG "MRVLOMXConfigParser"
#include <utils/Log.h>
#include "oscl_stdstring.h"

// Use default DLL entry point
#ifndef OSCL_DLL_H_INCLUDED
#include "oscl_dll.h"
#endif

#include "mrvl_omx_config_parser.h"
#include "pv_omxcore.h"
#include "mrvl_omx_dev.h"

OSCL_EXPORT_REF OMX_BOOL MRVLOMXConfigParser(
    OMX_PTR aInputParameters,
    OMX_PTR aOutputParameters)

{
    OMXConfigParserInputs* pInputs;

    pInputs = (OMXConfigParserInputs*) aInputParameters;

    for (int ii = 0; ii < NUM_OF_MRVL_VIDEO_PRI_COMP; ii++)
    {
	if (0 == strcmp(pInputs->cComponentName, mrvl_video_pri_comp[ii][0]))
	{
	    if ( 0 == access(mrvl_video_pri_comp[ii][1], F_OK))
	    {
		LOGD("can access %s", mrvl_video_pri_comp[ii][1]);
            }
            else
            {
            	// deactivate mved component if mved is not available
                LOGD("!!!can not access %s", mrvl_video_pri_comp[ii][1]);
	    }
	}
    }

    if (NULL != pInputs->cComponentRole)
    {
        if (0 == oscl_strncmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder", oscl_strlen("audio_decoder")))
        {
            OMX_S32 Status;
            pvAudioConfigParserInputs aInputs;

            aInputs.inPtr = pInputs->inPtr;
            aInputs.inBytes = pInputs->inBytes;

            if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.wma"))
            {
                aInputs.iMimeType = PVMF_MIME_WMA;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.aac"))
            {
                aInputs.iMimeType = PVMF_MIME_AAC_SIZEHDR;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.amr"))
            {
                aInputs.iMimeType = PVMF_MIME_AMR;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.amrnb"))
            {
                aInputs.iMimeType = PVMF_MIME_AMR;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.amrwb"))
            {
                aInputs.iMimeType = PVMF_MIME_AMRWB;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"audio_decoder.mp3"))
            {
                aInputs.iMimeType = PVMF_MIME_MP3;

            }
            else
            {
                return OMX_FALSE;
            }


            Status = pv_audio_config_parser(&aInputs, (pvAudioConfigParserOutputs *)aOutputParameters);
            if (0 == Status)
            {
                return OMX_FALSE;
            }
        }
        else if (0 == oscl_strncmp(pInputs->cComponentRole, (OMX_STRING)"video_decoder", oscl_strlen("video_decoder")))
        {

            OMX_S32 Status;
            pvVideoConfigParserInputs aInputs;

            aInputs.inPtr = pInputs->inPtr;
            aInputs.inBytes = pInputs->inBytes;

            if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"video_decoder.wmv"))
            {
                aInputs.iMimeType = PVMF_MIME_WMV;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"video_decoder.avc"))
            {
                aInputs.iMimeType = PVMF_MIME_H264_VIDEO;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"video_decoder.mpeg4"))
            {
                aInputs.iMimeType = PVMF_MIME_M4V;

            }
            else if (0 == oscl_strcmp(pInputs->cComponentRole, (OMX_STRING)"video_decoder.h263"))
            {
                aInputs.iMimeType = PVMF_MIME_H2632000;

            }
            else
            {
                return OMX_FALSE;
            }

            Status = mrvl_video_config_parser(&aInputs, (pvVideoConfigParserOutputs *)aOutputParameters);
            if (0 != Status)
            {
                return OMX_FALSE;
            }
        }
        else
        {
            return OMX_FALSE;
        }

    }
    else
    {
        return OMX_FALSE;
    }

    return OMX_TRUE;
}

