/***************************************************************************************** 
 * Copyright (c) 2009, Marvell International Ltd. 
 * All Rights Reserved.
 *****************************************************************************************/

#include "stagefright_mrvl_omx_plugin.h"
#include <media/stagefright/MediaDebug.h>
#include "OMX_IppDef.h"
#include <binder/IMemory.h>
#include <utils/RefBase.h>
#include <linux/android_pmem.h>
#include <sys/ioctl.h>

#define LOG_TAG "StageFright_HW"
//#define ENABLE_LOG
#ifdef ENABLE_LOG
#define MARVELL_LOG(...) LOGD(__VA_ARGS__)
#else
#define MARVELL_LOG(...) 
#endif

#define DEFAULT_AUDIO_DECODER_INPUT_BUFFER  (5)
#define DEFAULT_AUDIO_DECODER_OUTPUT_BUFFER (9)
#define DEFAULT_VIDEO_DECODER_INPUT_BUFFER  (5)
#define DEFAULT_VIDEO_DECODER_OUTPUT_BUFFER (5)
#define DEFAULT_AUDIO_ENCODER_INPUT_BUFFER (1)
#define DEFAULT_AUDIO_ENCODER_OUTPUT_BUFFER (12)

typedef struct{
    OMX_COMPONENTTYPE StandardComp;
    OMX_U8 ComponentName[128];
    OMX_CALLBACKTYPE  InternalCallBack;        
}IppOmxCompomentWrapper_t;

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
   
   {

   error = OMX_GetParameter(hWrapperHandle->pComponentPrivate, nParamIndex, pComponentParameterStructure);
   if (error != OMX_ErrorNone)
       return error;

   if (nParamIndex == OMX_IndexParamPortDefinition){
       OMX_PARAM_PORTDEFINITIONTYPE *pPort = (OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure;
       OMX_PARAM_COMPONENTROLETYPE Role;
       Role.nSize = sizeof(OMX_PARAM_COMPONENTROLETYPE);
       Role.nVersion = pPort->nVersion;
       
       OMX_STATETYPE nState;
       if(OMX_ErrorNone == OMX_GetState(hWrapperHandle->pComponentPrivate,&nState) && nState != OMX_StateLoaded){
            //if component state is not loaded , sill not set any parameter for component    
            return error;
       }
            
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

   if (!strcmp((char*)((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, "OMX.MARVELL.VIDEO.VMETAENCODER")  ||
       !strcmp((char*)((IppOmxCompomentWrapper_t*)hComponent)->ComponentName, "OMX.MARVELL.VIDEO.CODADX8ENCODER")) {
       if (pBuffer->pInputPortPrivate) {
           android::IMemory *mem = (android::IMemory*)(pBuffer->pInputPortPrivate);
           android::sp<android::IMemoryHeap> heap = mem->getMemory();
           void* va_base;
           OMX_S32 offset;
           struct pmem_region region;
           va_base = heap->getBase();
           if(ioctl(heap->getHeapID(), PMEM_GET_PHYS, &region) == 0) {
               offset = pBuffer->pBuffer + pBuffer->nOffset - (OMX_U8*)va_base;
               ((OMX_BUFFERHEADERTYPE_IPPEXT*)pBuffer)->nPhyAddr = (OMX_U32)(region.offset + offset);
               //LOGD("%s(%d): pa_base=%p va_base=%p pBuffer=%p offset=%d\n", __FUNCTION__, __LINE__, region.offset, va_base, pBuffer->pBuffer, offset);
           } else {
               ((OMX_BUFFERHEADERTYPE_IPPEXT*)pBuffer)->nPhyAddr = 0;
           }
           //LOGD("%s(%d): nPhyAddr=%p\n", __FUNCTION__, __LINE__, ((OMX_BUFFERHEADERTYPE_IPPEXT*)pBuffer)->nPhyAddr);
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


static OMX_ERRORTYPE _OMX_MasterInit()
{
    return OMX_Init();
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
static OMX_ERRORTYPE _OMX_MasterDeinit()
{
    return OMX_Deinit();
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

static OMX_ERRORTYPE _OMX_MasterGetHandle(
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
    
    
    pWrapperHandle = (IppOmxCompomentWrapper_t*)malloc(sizeof(IppOmxCompomentWrapper_t));
    if (pWrapperHandle){
        memset(pWrapperHandle, 0, sizeof(IppOmxCompomentWrapper_t));
        error = OMX_GetHandle(&pOmxInternalHandle, cComponentName, pAppData, &WrapperCallBack);
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

	    if (!strcmp(cComponentName, "OMX.MARVELL.AUDIO.AACENCODER")) {
		/*config aac encoder to output specific data for stagefright-based camcorder*/
		OMX_AUDIO_PARAM_MARVELL_AACENC par;
                par.nSize         	= sizeof(OMX_AUDIO_PARAM_MARVELL_AACENC);
	        par.nVersion.nVersion  	= 1;
                par.nPortIndex    	= 1;
                par.bOutputSpecificData = OMX_TRUE;
                error = OMX_SetParameter(pOmxInternalHandle, (OMX_INDEXTYPE)OMX_IndexParamMarvellAACEnc, &par);
            }

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
static OMX_ERRORTYPE _OMX_MasterFreeHandle(
    OMX_IN  OMX_HANDLETYPE hComponent)
{
    OMX_COMPONENTTYPE *hWrapperHandle = (OMX_COMPONENTTYPE*)(&(((IppOmxCompomentWrapper_t*)hComponent)->StandardComp));
    OMX_ERRORTYPE error = OMX_ErrorNone;

    MARVELL_LOG("OMX_FreeHandle %s", ((IppOmxCompomentWrapper_t*)hComponent)->ComponentName);
    error = OMX_FreeHandle(hWrapperHandle->pComponentPrivate);
    free(hComponent);
    return error;
    
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
static OMX_ERRORTYPE _OMX_MasterComponentNameEnum(
    OMX_OUT OMX_STRING cComponentName,
    OMX_IN  OMX_U32 nNameLength,
    OMX_IN  OMX_U32 nIndex)
{
    return OMX_ComponentNameEnum(cComponentName, nNameLength, nIndex);
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
static OMX_ERRORTYPE _OMX_MasterSetupTunnel(
    OMX_IN  OMX_HANDLETYPE hOutput,
    OMX_IN  OMX_U32 nPortOutput,
    OMX_IN  OMX_HANDLETYPE hInput,
    OMX_IN  OMX_U32 nPortInput)
{
    OMX_COMPONENTTYPE *pWrapperOutput = (OMX_COMPONENTTYPE*)hOutput;
    OMX_COMPONENTTYPE *pWrapperInput = (OMX_COMPONENTTYPE*)hInput;

    return OMX_SetupTunnel(pWrapperOutput->pComponentPrivate, nPortOutput, pWrapperInput->pComponentPrivate, nPortInput);

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
static OMX_ERRORTYPE _OMX_MasterGetRolesOfComponent ( 
    OMX_IN      OMX_STRING compName, 
    OMX_INOUT   OMX_U32 *pNumRoles,
    OMX_OUT     OMX_U8 **roles)
{   
    
    return OMX_GetRolesOfComponent(compName, pNumRoles, roles);
    
}

static OMX_ERRORTYPE _OMX_MasterGetComponentsOfRole ( 
    OMX_IN      OMX_STRING role,
    OMX_INOUT   OMX_U32 *pNumComps,
    OMX_INOUT   OMX_U8  **compNames)
{
    return OMX_GetComponentsOfRole(role, pNumComps, compNames);
}

static OMX_ERRORTYPE _OMX_MasterGetContentPipe( OMX_OUT OMX_HANDLETYPE *hPipe, OMX_IN OMX_STRING szURI )
{
    return OMX_GetContentPipe(hPipe, szURI);
}

namespace android {
OMXMRVLCodecsPlugin * createOMXPlugin(){
    return new OMXMRVLCodecsPlugin;
}

OMXMRVLCodecsPlugin::OMXMRVLCodecsPlugin() {
    _OMX_MasterInit();
}

OMXMRVLCodecsPlugin::~OMXMRVLCodecsPlugin() {
    _OMX_MasterDeinit();
}

OMX_ERRORTYPE OMXMRVLCodecsPlugin::makeComponentInstance(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component) {
    return _OMX_MasterGetHandle(
            reinterpret_cast<OMX_HANDLETYPE *>(component),
            const_cast<char *>(name),
            appData,
            const_cast<OMX_CALLBACKTYPE *>(callbacks));
}

OMX_ERRORTYPE OMXMRVLCodecsPlugin::destroyComponentInstance(
        OMX_COMPONENTTYPE *component) {
    return _OMX_MasterFreeHandle(component);
}

OMX_ERRORTYPE OMXMRVLCodecsPlugin::enumerateComponents(
        OMX_STRING name,
        size_t size,
        OMX_U32 index) {
    return _OMX_MasterComponentNameEnum(name, size, index);
}

OMX_ERRORTYPE OMXMRVLCodecsPlugin::getRolesOfComponent(
        const char *name,
        Vector<String8> *roles) {
    roles->clear();

    OMX_U32 numRoles;
    OMX_ERRORTYPE err =
        _OMX_MasterGetRolesOfComponent(
                const_cast<char *>(name),
                &numRoles,
                NULL);

    if (err != OMX_ErrorNone) {
        return err;
    }

    if (numRoles > 0) {
        OMX_U8 **array = new OMX_U8 *[numRoles];
        for (OMX_U32 i = 0; i < numRoles; ++i) {
            array[i] = new OMX_U8[OMX_MAX_STRINGNAME_SIZE];
        }

        OMX_U32 numRoles2=numRoles;
        err = _OMX_MasterGetRolesOfComponent(
                const_cast<char *>(name), &numRoles2, array);

        CHECK_EQ(err, OMX_ErrorNone);
        CHECK_EQ(numRoles, numRoles2);

        for (OMX_U32 i = 0; i < numRoles; ++i) {
            String8 s((const char *)array[i]);
            roles->push(s);

            delete[] array[i];
            array[i] = NULL;
        }

        delete[] array;
        array = NULL;
    }

    return OMX_ErrorNone;
}

}  // namespace android

