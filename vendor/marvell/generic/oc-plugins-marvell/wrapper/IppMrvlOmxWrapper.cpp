/***************************************************************************************** 
 *   Copyright (c) 2009, Marvell International Ltd. 
 *     All Rights Reserved.
 ****************************************************************************************/


#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_ContentPipe.h"
#include "OMX_IVCommon.h"
#include "OMX_Video.h"
#include "OMX_Audio.h"
#include "OMX_Image.h"
#include "OMX_Other.h"

#include "OMX_IppDef.h"

//#include "pvlogger.h"
#ifndef NO_OPENCORE
#include "pv_omxdefs.h"
#include "pv_omxcore.h"
#endif

#include <utils/Log.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/android_pmem.h>


#define LOG_TAG "MrvlOmxWrapper"

#define MRVL_OMX_CORE_LIBRARY "libMrvlOmx.so"

#define DEFAULT_AUDIO_DECODER_INPUT_BUFFER  (5)
#define DEFAULT_AUDIO_DECODER_OUTPUT_BUFFER (5)
#define DEFAULT_VIDEO_DECODER_INPUT_BUFFER  (5)
#define DEFAULT_VIDEO_DECODER_OUTPUT_BUFFER (5)
#define DEFAULT_AUDIO_ENCODER_INPUT_BUFFER (5)
#define DEFAULT_AUDIO_ENCODER_OUTPUT_BUFFER (32)

//#define ENABLE_LOG
#ifdef ENABLE_LOG
#define MARVELL_LOG(...) LOGD(__VA_ARGS__)
#else
#define MARVELL_LOG(...) 
#endif

typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_Init)(void) ;
typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_Deinit)(void);
typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_ComponentNameEnum)(
        OMX_OUT OMX_STRING cComponentName,
        OMX_IN  OMX_U32 nNameLength,
        OMX_IN  OMX_U32 nIndex);
typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_GetHandle)(
        OMX_OUT OMX_HANDLETYPE* pHandle,
        OMX_IN  OMX_STRING cComponentName,
        OMX_IN  OMX_PTR pAppData,
        OMX_IN  OMX_CALLBACKTYPE* pCallBacks);
typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_FreeHandle)(
        OMX_IN  OMX_HANDLETYPE hComponent);
typedef OMX_ERRORTYPE(*tpOMX_GetComponentsOfRole)(
        OMX_IN      OMX_STRING role,
        OMX_INOUT   OMX_U32 *pNumComps,
        OMX_INOUT   OMX_U8  **compNames);
typedef OMX_ERRORTYPE(*tpOMX_GetRolesOfComponent)(
        OMX_IN      OMX_STRING compName,
        OMX_INOUT   OMX_U32 *pNumRoles,
        OMX_OUT     OMX_U8 **roles);
typedef OMX_ERRORTYPE OMX_APIENTRY(*tpOMX_SetupTunnel)(
        OMX_IN  OMX_HANDLETYPE hOutput,
        OMX_IN  OMX_U32 nPortOutput,
        OMX_IN  OMX_HANDLETYPE hInput,
        OMX_IN  OMX_U32 nPortInput);
typedef OMX_ERRORTYPE(*tpOMX_GetContentPipe)(
        OMX_OUT OMX_HANDLETYPE *hPipe,
        OMX_IN OMX_STRING szURI);
typedef OMX_BOOL(*tpOMXConfigParser)(
        OMX_PTR aInputParameters,
        OMX_PTR aOutputParameters);
    
typedef struct{
    OMX_HANDLETYPE ipHandle;

    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_Init)(void);

    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_Deinit)(void);

    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_ComponentNameEnum)(
        OMX_OUT OMX_STRING cComponentName,
        OMX_IN  OMX_U32 nNameLength,
        OMX_IN  OMX_U32 nIndex);


    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_GetHandle)(
        OMX_OUT OMX_HANDLETYPE* pHandle,
        OMX_IN  OMX_STRING cComponentName,
        OMX_IN  OMX_PTR pAppData,
        OMX_IN  OMX_CALLBACKTYPE* pCallBacks);


    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_FreeHandle)(
        OMX_IN  OMX_HANDLETYPE hComponent);

    OMX_ERRORTYPE(*pOMX_GetComponentsOfRole)(
        OMX_IN      OMX_STRING role,
        OMX_INOUT   OMX_U32 *pNumComps,
        OMX_INOUT   OMX_U8  **compNames);


    OMX_ERRORTYPE(*pOMX_GetRolesOfComponent)(
        OMX_IN      OMX_STRING compName,
        OMX_INOUT   OMX_U32 *pNumRoles,
        OMX_OUT     OMX_U8 **roles);


    OMX_ERRORTYPE OMX_APIENTRY(*pOMX_SetupTunnel)(
        OMX_IN  OMX_HANDLETYPE hOutput,
        OMX_IN  OMX_U32 nPortOutput,
        OMX_IN  OMX_HANDLETYPE hInput,
        OMX_IN  OMX_U32 nPortInput);


    OMX_ERRORTYPE(*pOMX_GetContentPipe)(
        OMX_OUT OMX_HANDLETYPE *hPipe,
        OMX_IN OMX_STRING szURI);

    OMX_BOOL(*pOMXConfigParser)(
        OMX_PTR aInputParameters,
        OMX_PTR aOutputParameters);
}MrvlOmxWrapper_t;

typedef struct{
    OMX_COMPONENTTYPE StandardComp;
    OMX_U8 ComponentName[128];
    OMX_CALLBACKTYPE  InternalCallBack;        
    OMX_BOOL bUseExternalBuffer[4];
}IppOmxCompomentWrapper_t;

static MrvlOmxWrapper_t *g_MrvlOmxInterface=NULL;


// data structures for tunneling buffers
struct CAMERA_PMEM_INFO
{
        /* pmem file descriptor */
        uint32 pmem_fd;
        uint32 offset;
};

static void* getPA(void *private_info) {
    struct CAMERA_PMEM_INFO *info = (struct CAMERA_PMEM_INFO *)private_info;
    struct pmem_region region;
    if (private_info == NULL) return NULL;
    if(ioctl(info->pmem_fd, PMEM_GET_PHYS, &region) == 0) {
        return (void*)(region.offset + info->offset);
    }
    return NULL;
}

static OMX_ERRORTYPE IppOMXWrapper_GetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nParamIndex,  
        OMX_INOUT OMX_PTR pComponentParameterStructure){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   char ComponentName[128];
   OMX_VERSIONTYPE ComponentVersion;
   OMX_VERSIONTYPE SpecVersion;

   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   switch(nParamIndex){
#ifndef NO_OPENCORE
        case ((OMX_INDEXTYPE)PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX):
        {
            OMX_OTHER_PARAM_MARVELL_CAPABILITYTYPE MrvlCompCapFlags;
            char ComponentName[128];
            OMX_VERSIONTYPE ComponentVersion;
            OMX_VERSIONTYPE SpecVersion;
            PV_OMXComponentCapabilityFlagsType *pPVCompCapFlags = (PV_OMXComponentCapabilityFlagsType *)pComponentParameterStructure;
            
            OMX_GetComponentVersion(hWrapperHandle->pComponentPrivate, ComponentName, &ComponentVersion, &SpecVersion, NULL);
            MrvlCompCapFlags.nSize = sizeof(OMX_OTHER_PARAM_MARVELL_CAPABILITYTYPE); 
            MrvlCompCapFlags.nVersion.nVersion = ComponentVersion.nVersion;

            /* default setting begin */
            pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
            pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_FALSE;
            pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_FALSE;
            pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
            pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
            pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
            pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
            pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_FALSE;
            /* default setting end */

            if ((strcmp(ComponentName,"OMX.MARVELL.AUDIO.AACDECODER") == 0)
                ||(strcmp(ComponentName,"OMX.MARVELL.AUDIO.MP3DECODER") == 0))
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_FALSE;
            }else if (strcmp(ComponentName,"OMX.MARVELL.AUDIO.AMRNBDECODER") == 0){
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                  = OMX_FALSE;
            } 
            else if (strcmp(ComponentName,"OMX.MARVELL.VIDEO.H264DECODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_FALSE;
            }
            else if (strcmp(ComponentName,"OMX.MARVELL.VIDEO.H264ENCODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_TRUE;  
            }
            else if (strcmp(ComponentName,"OMX.MARVELL.VIDEO.MPEG4ASPDECODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                    = OMX_FALSE;
            }
            else if (strcmp(ComponentName,"OMX.MARVELL.VIDEO.H263DECODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                  = OMX_FALSE;
            }
            else if (strcmp(ComponentName, "OMX.MARVELL.VIDEO.VMETADECODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_TRUE;
            }
            else if (strcmp(ComponentName, "OMX.MARVELL.VIDEO.VMETAENCODER") == 0)
            {
                pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                     = OMX_FALSE;
            }
            else if (strcmp(ComponentName, "OMX.MARVELL.VIDEO.CODADX8DECODER") == 0)
            {
                 pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_FALSE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                 pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_FALSE;
                 pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                  = OMX_TRUE;
            }
            else if (strcmp(ComponentName, "OMX.MARVELL.AUDIO.AACENCODER") == 0) 
            {
                 pPVCompCapFlags->iIsOMXComponentMultiThreaded               = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_FALSE;
                 pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_FALSE;
                 pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                  = OMX_FALSE;
            }
            else if (strcmp(ComponentName, "OMX.MARVELL.VIDEO.CODADX8ENCODER") == 0)
            {
                 pPVCompCapFlags->iIsOMXComponentMultiThreaded                    = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc  = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc   = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers        = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentSupportsPartialFrames              = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentUsesNALStartCodes                  = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames          = OMX_TRUE;
                 pPVCompCapFlags->iOMXComponentUsesFullAVCFrames                  = OMX_TRUE;
            }
    
            MARVELL_LOG("%s capabilty: \n MultiThread - %s,\n ExternalOutputBufferAlloc - %s,\n ExternalInputBufferAlloc - %s,\n MovableInputBuffers - %s,\n SupportsPartialFrames - %s,\n UsesNALStartCodes - %s,\n HandleIncompleteFrames - %s,\n UsesFullAVCFrames - %s\n",ComponentName, \
                pPVCompCapFlags->iIsOMXComponentMultiThreaded?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentSupportsExternalOutputBufferAlloc?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentSupportsExternalInputBufferAlloc?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentSupportsMovableInputBuffers?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentSupportsPartialFrames?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentUsesNALStartCodes?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentCanHandleIncompleteFrames?"Yes":"No", \
                pPVCompCapFlags->iOMXComponentUsesFullAVCFrames?"Yes":"No");
            break;
        }
#endif
        default :
        {
            error = OMX_GetParameter(hWrapperHandle->pComponentPrivate, nParamIndex, pComponentParameterStructure);
            if (error != OMX_ErrorNone)
                return error;

            if (nParamIndex == OMX_IndexParamPortDefinition){
            OMX_PARAM_PORTDEFINITIONTYPE *pPort = (OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure;
            OMX_PARAM_COMPONENTROLETYPE Role;
            Role.nSize = sizeof(OMX_PARAM_COMPONENTROLETYPE);
            Role.nVersion = pPort->nVersion;
                    
            OMX_GetParameter(hWrapperHandle->pComponentPrivate, OMX_IndexParamStandardComponentRole, &Role);
            if(!strncmp((char*)((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, "OMX.MARVELL.AUDIO.AMRNBENCODER", strlen("OMX.MARVELL.AUDIO.AMRNBENCODER"))){
                 if (pPort->eDir == OMX_DirInput){
                    if (pPort->nBufferCountActual < DEFAULT_AUDIO_ENCODER_INPUT_BUFFER){
                        pPort->nBufferCountActual = DEFAULT_AUDIO_ENCODER_INPUT_BUFFER;
                    }
                } else if (pPort->eDir == OMX_DirOutput){
                    if (pPort->nBufferCountActual < DEFAULT_AUDIO_ENCODER_OUTPUT_BUFFER){
                        pPort->nBufferCountActual = DEFAULT_AUDIO_ENCODER_OUTPUT_BUFFER;
                    }
                }  
            } 
            if (!strncmp((char *)Role.cRole, "audio_decoder", strlen("audio_decoder"))){
                if (pPort->eDir == OMX_DirInput){
                    if (pPort->nBufferCountActual < DEFAULT_AUDIO_DECODER_INPUT_BUFFER){
                        pPort->nBufferCountActual = DEFAULT_AUDIO_DECODER_INPUT_BUFFER;
                    }
                } else if (pPort->eDir == OMX_DirOutput){
                    if (pPort->nBufferCountActual < DEFAULT_AUDIO_DECODER_OUTPUT_BUFFER){
                        pPort->nBufferCountActual = DEFAULT_AUDIO_DECODER_OUTPUT_BUFFER;
                    }
                }
            } else if (!strncmp((char *)Role.cRole, "video_decoder", strlen("video_decoder"))){
                if (pPort->eDir == OMX_DirInput){
                    if (pPort->nBufferCountActual < DEFAULT_VIDEO_DECODER_INPUT_BUFFER){
                        pPort->nBufferCountActual = DEFAULT_VIDEO_DECODER_INPUT_BUFFER;
                    }
                } else if (pPort->eDir == OMX_DirOutput) {
                    if (pPort->nBufferCountActual < DEFAULT_VIDEO_DECODER_OUTPUT_BUFFER) {
                        pPort->nBufferCountActual = DEFAULT_VIDEO_DECODER_OUTPUT_BUFFER;
                    }
                }
            } /*else if (!strncmp((char *)Role.cRole,"audio_encoder", strlen("audio_encoder"))){
                   
            }else if (!strncmp((char *)Role.cRole,"video_encoder", strlen("video_encoder"))){

            }
               */
            /*update the modified info for component.*/
            error = OMX_SetParameter(hWrapperHandle->pComponentPrivate, nParamIndex, pComponentParameterStructure);
            if (error != OMX_ErrorNone)
                return error;

            }
            break;
       }
   }        
   return error;
}


/** refer to OMX_SetParameter in OMX_core.h or the OMX IL 
    specification for details on the SetParameter method.
 */
static OMX_ERRORTYPE IppOMXWrapper_SetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nIndex,
        OMX_IN  OMX_PTR pComponentParameterStructure){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   error = OMX_SetParameter(hWrapperHandle->pComponentPrivate, nIndex, pComponentParameterStructure);
   
   return error;
}


/** refer to OMX_GetConfig in OMX_core.h or the OMX IL 
    specification for details on the GetConfig method.
 */
static OMX_ERRORTYPE IppOMXWrapper_GetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_INOUT OMX_PTR pComponentConfigStructure){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   return OMX_GetConfig(hWrapperHandle->pComponentPrivate, nIndex, pComponentConfigStructure);
}


/** refer to OMX_SetConfig in OMX_core.h or the OMX IL 
    specification for details on the SetConfig method.
 */
static OMX_ERRORTYPE IppOMXWrapper_SetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_IN  OMX_PTR pComponentConfigStructure){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   return OMX_SetConfig(hWrapperHandle->pComponentPrivate, nIndex, pComponentConfigStructure);
}

/** refer to OMX_GetExtensionIndex in OMX_core.h or the OMX IL 
    specification for details on the GetExtensionIndex method.
 */
static OMX_ERRORTYPE IppOMXWrapper_GetExtensionIndex(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_STRING cParameterName,
        OMX_OUT OMX_INDEXTYPE* pIndexType){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   return OMX_GetExtensionIndex(hWrapperHandle->pComponentPrivate, cParameterName, pIndexType);
}

static OMX_ERRORTYPE IppOMXWrapper_UseBuffer(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_IN OMX_U32 nPortIndex,
        OMX_IN OMX_PTR pAppPrivate,
        OMX_IN OMX_U32 nSizeBytes,
        OMX_IN OMX_U8* pBuffer){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   if (nPortIndex < sizeof(((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer)/sizeof(OMX_BOOL)){
       ((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer[nPortIndex] = OMX_TRUE;
   }
   
   //OMX_GetComponentVersion(hWrapperHandle->pComponentPrivate, ComponentName, &ComponentVersion, &SpecVersion, NULL);
   MARVELL_LOG("%s : OMX_UseBuffer port=%u size=%u addr=%p", ((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, nPortIndex, nSizeBytes, pBuffer);    
   return OMX_UseBuffer(hWrapperHandle->pComponentPrivate, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}
/** refer to OMX_AllocateBuffer in OMX_core.h or the OMX IL 
    specification for details on the AllocateBuffer method.
    @ingroup buf
 */
static OMX_ERRORTYPE IppOMXWrapper_AllocateBuffer(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
        OMX_IN OMX_U32 nPortIndex,
        OMX_IN OMX_PTR pAppPrivate,
        OMX_IN OMX_U32 nSizeBytes){
    OMX_COMPONENTTYPE *hWrapperHandle = NULL;
    OMX_ERRORTYPE error = OMX_ErrorNone;
 
    if (hComponent == NULL){
         return OMX_ErrorInvalidComponent;
    }
    if (nPortIndex < sizeof(((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer)/sizeof(OMX_BOOL)){
        ((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer[nPortIndex] = OMX_FALSE;
    }
    hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
    //OMX_GetComponentVersion(hWrapperHandle->pComponentPrivate, ComponentName, &ComponentVersion, &SpecVersion, NULL);
    MARVELL_LOG("%s : OMX_AllocateBuffer port=%d size=%u", ((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, nPortIndex, nSizeBytes);
     
     return OMX_AllocateBuffer(hWrapperHandle->pComponentPrivate, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
}

/** refer to OMX_FreeBuffer in OMX_core.h or the OMX IL 
    specification for details on the FreeBuffer method.
    @ingroup buf
 */
static OMX_ERRORTYPE IppOMXWrapper_FreeBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   MARVELL_LOG("%s: OMX_FreeBuffer: port %d , ptr: 0x%x",((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, nPortIndex,pBuffer->pBuffer);
   error = OMX_FreeBuffer(hWrapperHandle->pComponentPrivate, nPortIndex, pBuffer);
   if (error != OMX_ErrorNone){
        LOGE("%s: OMX_FreeBuffer Failed: port %u , ptr: 0x%x",((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, nPortIndex, pBuffer->pBuffer);
   }
   return error;
}

/** refer to OMX_EmptyThisBuffer in OMX_core.h or the OMX IL 
    specification for details on the EmptyThisBuffer method.
    @ingroup buf
 */
static OMX_ERRORTYPE IppOMXWrapper_EmptyThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;

   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
            
   if (!strcmp((const char *)((IppOmxCompomentWrapper_t*)hComponent)->ComponentName,"OMX.MARVELL.VIDEO.VMETAENCODER") || 
       !strcmp((const char *)((IppOmxCompomentWrapper_t*)hComponent)->ComponentName,"OMX.MARVELL.VIDEO.CODADX8ENCODER")){
        //LOGD("get the PMEM physical address 0x%x",(*(unsigned int *)pBuffer->pBuffer));
        if ( pBuffer->nInputPortIndex < sizeof(((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer)/sizeof(OMX_BOOL) && 
             ((IppOmxCompomentWrapper_t*)hComponent)->bUseExternalBuffer[pBuffer->nInputPortIndex] == OMX_TRUE){
            ((OMX_BUFFERHEADERTYPE_IPPEXT*)pBuffer)->nPhyAddr = (OMX_U32)getPA(pBuffer->pPlatformPrivate);//(*(unsigned int *)pBuffer->pBuffer);
            if (((OMX_BUFFERHEADERTYPE_IPPEXT*)pBuffer)->nPhyAddr == 0){
                LOGE("The Physical address for HW encoder is illegal NULL");
            }
        }
   }
   //OMX_GetComponentVersion(hWrapperHandle->pComponentPrivate, ComponentName, &ComponentVersion, &SpecVersion, NULL);
   //MARVELL_LOG("%s: OMX_EmptyThisBuffer ",((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, );
   return OMX_EmptyThisBuffer(hWrapperHandle->pComponentPrivate, pBuffer);
}

/** refer to OMX_FillThisBuffer in OMX_core.h or the OMX IL 
    specification for details on the FillThisBuffer method.
    @ingroup buf
 */
static OMX_ERRORTYPE IppOMXWrapper_FillThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;

   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));           
   //MARVELL_LOG("%s: OMX_FillThisBuffer", ((IppOmxCompomentWrapper_t*)hComponent)->ComponentName,);
   return OMX_FillThisBuffer(hWrapperHandle->pComponentPrivate, pBuffer);
}

static OMX_ERRORTYPE IppOMXWrapper_GetComponentVersion(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STRING pComponentName,
        OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
        OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
        OMX_OUT OMX_UUIDTYPE* pComponentUUID){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   return OMX_GetComponentVersion(hWrapperHandle->pComponentPrivate, pComponentName,pComponentVersion, pSpecVersion, pComponentUUID);
}

/** refer to OMX_SendCommand in OMX_core.h or the OMX IL 
    specification for details on the SendCommand method.
 */
static OMX_ERRORTYPE IppOMXWrapper_SendCommand(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_COMMANDTYPE Cmd,
        OMX_IN  OMX_U32 nParam1,
        OMX_IN  OMX_PTR pCmdData){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   LOGD("%s: OMX_SendCommand: cmd: %d, nParam1: %d ",((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, Cmd, nParam1);
   error = OMX_SendCommand(hWrapperHandle->pComponentPrivate, Cmd, nParam1, pCmdData);
   return error;
}

/** refer to OMX_GetState in OMX_core.h or the OMX IL 
    specification for details on the GetState method.
 */
static OMX_ERRORTYPE IppOMXWrapper_GetState(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STATETYPE* pState){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   return OMX_GetState(hWrapperHandle->pComponentPrivate, pState);
}

/** @ingroup buf */
static OMX_ERRORTYPE IppOMXWrapper_UseEGLImage(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_IN OMX_U32 nPortIndex,
        OMX_IN OMX_PTR pAppPrivate,
        OMX_IN void* eglImage){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   
   return OMX_UseEGLImage(hWrapperHandle->pComponentPrivate, ppBufferHdr,nPortIndex,pAppPrivate,eglImage);
}
static     OMX_ERRORTYPE IppOMXWrapper_ComponentRoleEnum(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_U8 *cRole,
        OMX_IN OMX_U32 nIndex){
   OMX_COMPONENTTYPE *hWrapperHandle = NULL;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   OMX_COMPONENTTYPE *hInternalComp = NULL;

   if (hComponent == NULL){
        return OMX_ErrorInvalidComponent;
   }
   hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
   hInternalComp = (OMX_COMPONENTTYPE*)hWrapperHandle->pComponentPrivate;
   return hInternalComp->ComponentRoleEnum(hInternalComp, cRole, nIndex);
}

static OMX_ERRORTYPE IppOMXWrapper_EventHandler(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1,
        OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData){
   IppOmxCompomentWrapper_t *hWrapperHandle = (IppOmxCompomentWrapper_t*)((OMX_COMPONENTTYPE*)hComponent)->pApplicationPrivate;
   OMX_ERRORTYPE error;
   if (hWrapperHandle == NULL){
        return OMX_ErrorInvalidComponent;
   }
   error = hWrapperHandle->InternalCallBack.EventHandler(hWrapperHandle, pAppData, eEvent, nData1, nData2, pEventData);
   LOGD("%s: Event: %d, nData1: %d, nData2: %d",hWrapperHandle->ComponentName, eEvent, nData1, nData2);

   return error;
}

  
static OMX_ERRORTYPE IppOMXWrapper_EmptyBufferDone(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer){
   IppOmxCompomentWrapper_t *hWrapperHandle = (IppOmxCompomentWrapper_t*)((OMX_COMPONENTTYPE*)hComponent)->pApplicationPrivate;
   OMX_ERRORTYPE error;

   if (hWrapperHandle == NULL){
        return OMX_ErrorInvalidComponent;
   }
   MARVELL_LOG("%s: EmptyBufferDone ", hWrapperHandle->ComponentName);
   return hWrapperHandle->InternalCallBack.EmptyBufferDone(hWrapperHandle, pAppData, pBuffer);
}

 
static OMX_ERRORTYPE IppOMXWrapper_FillBufferDone(
        OMX_OUT OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_PTR pAppData,
        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer){
   IppOmxCompomentWrapper_t *hWrapperHandle = (IppOmxCompomentWrapper_t*)((OMX_COMPONENTTYPE*)hComponent)->pApplicationPrivate;
   OMX_ERRORTYPE error;
    if (hWrapperHandle == NULL){
        return OMX_ErrorInvalidComponent;
   }
   MARVELL_LOG("%s: FillBufferDone", hWrapperHandle->ComponentName);
   return hWrapperHandle->InternalCallBack.FillBufferDone(hWrapperHandle, pAppData, pBuffer);
       
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init()
{
    OMX_HANDLETYPE ipHandle=NULL;


    if (g_MrvlOmxInterface == NULL){
        g_MrvlOmxInterface = (MrvlOmxWrapper_t*)malloc(sizeof(MrvlOmxWrapper_t));
        if (g_MrvlOmxInterface == NULL){
            return OMX_ErrorInsufficientResources;
        }
        g_MrvlOmxInterface->pOMX_Init = NULL;
        g_MrvlOmxInterface->pOMX_Deinit = NULL;
        g_MrvlOmxInterface->pOMX_ComponentNameEnum = NULL;
        g_MrvlOmxInterface->pOMX_GetHandle = NULL;
        g_MrvlOmxInterface->pOMX_FreeHandle = NULL;
        g_MrvlOmxInterface->pOMX_GetComponentsOfRole = NULL;
        g_MrvlOmxInterface->pOMX_GetRolesOfComponent = NULL;
        g_MrvlOmxInterface->pOMX_SetupTunnel = NULL;
        g_MrvlOmxInterface->pOMX_GetContentPipe = NULL;
        g_MrvlOmxInterface->pOMXConfigParser = NULL;
        g_MrvlOmxInterface->ipHandle = NULL;
        
        ipHandle = dlopen(MRVL_OMX_CORE_LIBRARY, RTLD_NOW);
        if (ipHandle == NULL){
            LOGE("******dlopen %s error %s", MRVL_OMX_CORE_LIBRARY, dlerror());
            free(g_MrvlOmxInterface);
            g_MrvlOmxInterface = NULL;
            return OMX_ErrorInsufficientResources;
        }else{
            // Lookup all the symbols in the OMX core
            g_MrvlOmxInterface->ipHandle = ipHandle;
            g_MrvlOmxInterface->pOMX_Init = (tpOMX_Init)dlsym(ipHandle, "OMX_Init");
            g_MrvlOmxInterface->pOMX_Deinit = (tpOMX_Deinit)dlsym(ipHandle, "OMX_Deinit");
            g_MrvlOmxInterface->pOMX_ComponentNameEnum = (tpOMX_ComponentNameEnum)dlsym(ipHandle, "OMX_ComponentNameEnum");
            g_MrvlOmxInterface->pOMX_GetHandle = (tpOMX_GetHandle)dlsym(ipHandle, "OMX_GetHandle");
            g_MrvlOmxInterface->pOMX_FreeHandle = (tpOMX_FreeHandle)dlsym(ipHandle, "OMX_FreeHandle");
            g_MrvlOmxInterface->pOMX_GetComponentsOfRole = (tpOMX_GetComponentsOfRole)dlsym(ipHandle, "OMX_GetComponentsOfRole");
            g_MrvlOmxInterface->pOMX_GetRolesOfComponent = (tpOMX_GetRolesOfComponent)dlsym(ipHandle, "OMX_GetRolesOfComponent");
            g_MrvlOmxInterface->pOMX_SetupTunnel = (tpOMX_SetupTunnel)dlsym(ipHandle, "OMX_SetupTunnel");
            g_MrvlOmxInterface->pOMX_GetContentPipe = (tpOMX_GetContentPipe)dlsym(ipHandle, "OMX_GetContentPipe");
            MARVELL_LOG("OMX symbols in %s are hooked up", MRVL_OMX_CORE_LIBRARY);
            //g_MrvlOmxInterface->pOMXConfigParser = (tpOMXConfigParser)dlsym(ipHandle, "OMXConfigParser");
        }
        if (g_MrvlOmxInterface->pOMX_Init){
            MARVELL_LOG("MRVL OMX Core is initializing ...");
            return g_MrvlOmxInterface->pOMX_Init();
        }else{
            LOGE("******dlsym OMX_Init in %s error", MRVL_OMX_CORE_LIBRARY);
            free(g_MrvlOmxInterface);
            g_MrvlOmxInterface = NULL;
            return OMX_ErrorInsufficientResources;
        }
        
    }else{
        LOGE("***** OMX CORE is already initialized inst=%p ******", g_MrvlOmxInterface);
    /*
        if (g_MrvlOmxInterface->pOMX_Init){
            return g_MrvlOmxInterface->pOMX_Init();
        }else{
            free(g_MrvlOmxInterface);
            g_MrvlOmxInterface = NULL;
            return OMX_ErrorInsufficientResources;
        }*/
        return OMX_ErrorNone;
    }

}

/***************************************************************************
// Name:             OMX_Deinit
// Description:      Deinitialize the OMX core. The last call made into OMX.
// Call Type:         Blocking Call. Must respond within 20 msec
//
// Input Arguments:
//                        None
// Output Arguments:
//                        None
// Returns:          
//                        OMX_ErrorNone:    Function Successfully Executed
//                        OMX_ErrorVersionMismatch: OMX core version mismatch
*****************************************************************************/
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit()
{
    OMX_ERRORTYPE err;
    
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_Deinit){
        MARVELL_LOG("MRVL OMX Core is de-initializing...");
        err = g_MrvlOmxInterface->pOMX_Deinit();
        if (g_MrvlOmxInterface->ipHandle){
            dlclose(g_MrvlOmxInterface->ipHandle);
        }
        free(g_MrvlOmxInterface);
        g_MrvlOmxInterface = NULL;
        return err;
    }else{
        return OMX_ErrorNone;
    }
}

/***************************************************************************
// Name:             OMX_GetHandle
// Description:      Create a component handle.
// Call Type:         Blocking Call. Must respond within 20 msec
//
// Input Arguments:
//        cComponentName    - Pointer to a null terminated string with the component name with
//                          following format: "OMX.<vendor_name>.<vendor_specified_convention>"
//        pAppData        - Pointer to an application defined value so that the
//                          application can identify the source of the callback
//        pCallBacks        - pointer to a OMX_CALLBACKTYPE structure that will be 
//                          passed to the component to initialize it with
// Output Arguments:
//        pHandle            - Pointer to an OMX_HANDLETYPE pointer to be filled
// Returns:          
//            OMX_ErrorNone:    Function Successfully Executed
//            OMX_ErrorInsufficientResources:    Allocate OMX_COMPONENTTYPE structure failed
//            OMX_ErrorComponentNotFound: OMX Core didn't support this type of component
//            OMX_ErrorUndefined: OMX_ComponentInit failed
//            OMX_ErrorInvalidComponentName: The component name is not valid
//            OMX_ErrorInvalidComponent: The component specified didn't have a "OMX_ComponentInit" entry 
//            OMX_ErrorBadParameter: Callback function entries error
//            OMX_ErrorVersionMismatch: OMX component version mismatch
*****************************************************************************/

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE* pHandle, 
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
{
    IppOmxCompomentWrapper_t *pWrapperHandle=NULL;
    OMX_HANDLETYPE pOmxInternalHandle = NULL;
    OMX_ERRORTYPE error=OMX_ErrorNone;
    OMX_CALLBACKTYPE WrapperCallBack;

    WrapperCallBack.EmptyBufferDone = IppOMXWrapper_EmptyBufferDone;
    WrapperCallBack.FillBufferDone  = IppOMXWrapper_FillBufferDone;
    WrapperCallBack.EventHandler    = IppOMXWrapper_EventHandler;
    
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_GetHandle){
        pWrapperHandle = (IppOmxCompomentWrapper_t*)malloc(sizeof(IppOmxCompomentWrapper_t));
        if (pWrapperHandle){
            memset(pWrapperHandle, 0, sizeof(IppOmxCompomentWrapper_t));
            error = g_MrvlOmxInterface->pOMX_GetHandle(&pOmxInternalHandle, cComponentName, pAppData, &WrapperCallBack);
            if (error == OMX_ErrorNone){
                *pHandle = (OMX_HANDLETYPE)pWrapperHandle;
                /*override function*/
                 pWrapperHandle->StandardComp.GetComponentVersion        = IppOMXWrapper_GetComponentVersion;
                 pWrapperHandle->StandardComp.SendCommand                = IppOMXWrapper_SendCommand;
                 pWrapperHandle->StandardComp.GetParameter            = IppOMXWrapper_GetParameter;
                 pWrapperHandle->StandardComp.SetParameter            = IppOMXWrapper_SetParameter;
                 pWrapperHandle->StandardComp.GetConfig                = IppOMXWrapper_GetConfig;
                 pWrapperHandle->StandardComp.SetConfig                = IppOMXWrapper_SetConfig;
                 pWrapperHandle->StandardComp.GetExtensionIndex        = IppOMXWrapper_GetExtensionIndex;
                 pWrapperHandle->StandardComp.GetState                = IppOMXWrapper_GetState;
                 pWrapperHandle->StandardComp.UseBuffer                = IppOMXWrapper_UseBuffer;
                 pWrapperHandle->StandardComp.AllocateBuffer            = IppOMXWrapper_AllocateBuffer;
                 pWrapperHandle->StandardComp.FreeBuffer                = IppOMXWrapper_FreeBuffer;
                 pWrapperHandle->StandardComp.EmptyThisBuffer            = IppOMXWrapper_EmptyThisBuffer;
                 pWrapperHandle->StandardComp.FillThisBuffer            = IppOMXWrapper_FillThisBuffer;
                 pWrapperHandle->StandardComp.UseEGLImage                = IppOMXWrapper_UseEGLImage;
                 pWrapperHandle->StandardComp.ComponentRoleEnum        = IppOMXWrapper_ComponentRoleEnum;

                pWrapperHandle->InternalCallBack.EmptyBufferDone = pCallBacks->EmptyBufferDone;
                pWrapperHandle->InternalCallBack.FillBufferDone = pCallBacks->FillBufferDone;
                pWrapperHandle->InternalCallBack.EventHandler = pCallBacks->EventHandler;
                

                ((OMX_COMPONENTTYPE*)pOmxInternalHandle)->pApplicationPrivate = pWrapperHandle;
                pWrapperHandle->StandardComp.pComponentPrivate        = pOmxInternalHandle;
                strncpy((char*)pWrapperHandle->ComponentName, cComponentName, sizeof(pWrapperHandle->ComponentName));
                MARVELL_LOG("OMX_GetHandle succeeded and exiting %s", pWrapperHandle->ComponentName);
                return error;
            }else{
                LOGE("OMX_GetHandle failed %s (ret=0x%x)", cComponentName, error);
                free(pWrapperHandle);
                *pHandle = NULL;
                return error;
            }
            
        } else {
            return OMX_ErrorInsufficientResources;
    }
    }else{
        LOGE("g_MrvlOmxInterface is not initialized yet!!!");
        return OMX_ErrorInsufficientResources;
    }
}

/***************************************************************************
// Name:             OMX_FreeHandle
// Description:      Free a component handle.
// Call Type:         Blocking Call. Must respond within 20 msec
//
// Input Arguments:
//        hComponent        - Handle of the component to be accessed
// Output Arguments:
//                        None
// Returns:          
//            OMX_ErrorNone:    Function Successfully Executed
//            OMX_ErrorIncorrectStateOperation: Component is NOT in the loaded or invalid state
//            OMX_ErrorUndefined: Free resources failed
//            OMX_ErrorInvalidComponent: The component specified didn't have a "OMX_ComponentDeinit" entry 
//            OMX_ErrorVersionMismatch: OMX component version mismatch
*****************************************************************************/
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(
    OMX_IN  OMX_HANDLETYPE hComponent)
{
        OMX_COMPONENTTYPE *hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
    OMX_ERRORTYPE error = OMX_ErrorNone;

    
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_FreeHandle){
        MARVELL_LOG("OMX_FreeHandle %s", ((IppOmxCompomentWrapper_t*)hComponent)->ComponentName);
        error = g_MrvlOmxInterface->pOMX_FreeHandle(hWrapperHandle->pComponentPrivate);
        free(hComponent);
        return error;
    }else{
        return OMX_ErrorInvalidComponent;
    }
}

/***************************************************************************
// Name:             OMX_ComponentNameEnum
// Description:      Enumerate through all the names of recognized valid 
//                     components in the system
// Call Type:         Blocking Call. 
//
// Input Arguments:
//        nNameLength        - Number of characters in the cComponentName string
//        nIndex            - Number containing the enumeration index for the component
// Output Arguments:
//        cComponentName    - Pointer to a null terminated string with the component name
// Returns:          
//            OMX_ErrorNone:    Function Successfully Executed
//            OMX_ErrorNoMore:  nIndex exceeds the number of components in the system minus 1
//            OMX_ErrorUndefined: Fill in cComponentName failed
*****************************************************************************/
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_ComponentNameEnum(
    OMX_OUT OMX_STRING cComponentName,
    OMX_IN  OMX_U32 nNameLength,
    OMX_IN  OMX_U32 nIndex)
{
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_ComponentNameEnum){
        return g_MrvlOmxInterface->pOMX_ComponentNameEnum(cComponentName, nNameLength, nIndex);
    }else{
        return OMX_ErrorInvalidComponent;
    }
}

/***************************************************************************
// Name:             OMX_SetupTunnel
// Description:      Setup the specified path between the two components
// Call Type:         Blocking Call. should return within 20 ms
//
// Input Arguments:
//        hOutput            - Handle of the component to be accessed.  It holds the  
//                          port to be specified in the nPortOutput parameter.
//                          It is required that hOutput be the source for the data 
//        nPortOutput        - Indicate the source port on component to be used in the tunnel
//        hInput            - Handle of he component to setup the tunnel with.  It holds
//                          the port to be specified in the nPortInput parameter.
//                          It is required that hInput be the destination for the data 
//        nPortInput        - Indicate the destination port on component to be used in the tunnel
// Output Arguments:
//                        None
// Returns:          
//        OMX_ErrorNone:    Function Successfully Executed
//        OMX_ErrorBadParameter: Input parameter is not valid
//        OMX_ErrorInvalidState: Component is in invalid state
//        OMX_ErrorPortsNotCompatible: Ports being connected are not compatible
//        OMX_ErrorVersionMismatch: The component version didn't match
*****************************************************************************/
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(
    OMX_IN  OMX_HANDLETYPE hOutput,
    OMX_IN  OMX_U32 nPortOutput,
    OMX_IN  OMX_HANDLETYPE hInput,
    OMX_IN  OMX_U32 nPortInput)
{
    OMX_COMPONENTTYPE *pWrapperOutput = (OMX_COMPONENTTYPE*)hOutput;
    OMX_COMPONENTTYPE *pWrapperInput = (OMX_COMPONENTTYPE*)hInput;

    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_SetupTunnel){
        return g_MrvlOmxInterface->pOMX_SetupTunnel(pWrapperOutput->pComponentPrivate, nPortOutput, pWrapperInput->pComponentPrivate, nPortInput);
    }else{
        return OMX_ErrorInvalidComponent;
    }
}


/***************************************************************************
// Name:             OMX_GetRolesOfcomponent
// Description:      query all the roles fulfilled by a given component
// Call Type:         Blocking Call. should return within 5 ms
//
// Input Arguments:
//        compName        - Name of the component being queried about  
// InOut Arguments:
//        pNumRoles:        - On input it bounds the size of the input structure. 
//                        - On output it specifies how many roles were retrieved.
//        roles:            - List of the names of all standard components implemented on the 
//                          specified physical component name. 
//                          If this pointer is NULL this function populates the pNumRoles 
//                          field with the number of roles the component supports and 
//                          ignores the roles field. 
// Output Arguments:
//                        None
// Returns:          
//        OMX_ErrorNone:    Function Successfully Executed
*****************************************************************************/
OMX_API OMX_ERRORTYPE OMX_GetRolesOfComponent ( 
    OMX_IN      OMX_STRING compName, 
    OMX_INOUT   OMX_U32 *pNumRoles,
    OMX_OUT     OMX_U8 **roles)
{   
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_GetRolesOfComponent){
        return g_MrvlOmxInterface->pOMX_GetRolesOfComponent(compName, pNumRoles, roles);
    }else{
        return OMX_ErrorInvalidComponent;
    }
}

OMX_API OMX_ERRORTYPE OMX_GetComponentsOfRole ( 
    OMX_IN      OMX_STRING role,
    OMX_INOUT   OMX_U32 *pNumComps,
    OMX_INOUT   OMX_U8  **compNames)
{
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_GetComponentsOfRole){
        return g_MrvlOmxInterface->pOMX_GetComponentsOfRole(role, pNumComps, compNames);
    }else{
        return OMX_ErrorInvalidComponent;
    }
}

OMX_API OMX_ERRORTYPE OMX_GetContentPipe( OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI )
{
    if (g_MrvlOmxInterface && g_MrvlOmxInterface->pOMX_GetContentPipe){
        return g_MrvlOmxInterface->pOMX_GetContentPipe(hPipe, szURI);
    }else{
        return OMX_ErrorInvalidComponent;
    }
}

