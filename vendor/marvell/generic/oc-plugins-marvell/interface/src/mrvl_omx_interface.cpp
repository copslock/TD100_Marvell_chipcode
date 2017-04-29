/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
/***************************************************************************************** 
  Copyright (c) 2009, Marvell International Ltd. 
  All Rights Reserved.
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
//#define LOG_NDEBUG 0
#define LOG_TAG "omx_mrvl"
#include <utils/Log.h>

#include "pvlogger.h"

#include "pv_omxcore.h"
#include "omx_interface.h"

#define OMX_CORE_WRAPPER_LIBRARY "libMrvlOmxWrapper.so"
#define OMX_CORE_LIBRARY "libMrvlOmx.so"
#define OMX_CONFIG_LIBRARY "libMrvlOmxConfig.so"

class MRVLOMXInterface : public OMXInterface
{
    public:
        // Handle to the OMX core library
        void* ipHandle;

	// Handle to the OMXConfig library
	void* ipConfigHandle;

        ~MRVLOMXInterface()
        {
            if ((NULL != ipHandle) && (0 != dlclose(ipHandle)))
            {
                // dlclose() returns non-zero value if close failed, check for errors
                const char* pErr = dlerror();
                if (NULL != pErr)
                {
                    LOGE("MRVLOMXInterface: Error closing library: %s\n", pErr);
                }
                else
                {
                    LOGE("MRVLOMXInterface::Close: Error closing library, no error reported");
                }
            }

            if ((NULL != ipConfigHandle) && (0 != dlclose(ipConfigHandle)))
            {
                // dlclose() returns non-zero value if close failed, check for errors
                const char* pErr = dlerror();
                if (NULL != pErr)
                {
                    LOGE("MRVLOMXInterface: Error closing library: %s\n", pErr);
                }
                else
                {
                    LOGE("MRVLOMXInterface::Close: Error closing library, no error reported");
                }
            }

            ipHandle = NULL;
	    ipConfigHandle = NULL;
        };

        OsclAny* SharedLibraryLookup(const OsclUuid& aInterfaceId)
        {
            // Make sure ipHandle is valid. If ipHandle is NULL, the dlopen
            // call failed.
            if (ipHandle && aInterfaceId == OMX_INTERFACE_ID)
            {
                // the library lookup was successful
                return this;
            }
            // the ID doesn't match
            return NULL;
        };

        static MRVLOMXInterface* Instance()
        {
            return OSCL_NEW(MRVLOMXInterface, ());
        };

        bool UnloadWhenNotUsed(void)
        {
            return false;
        };

    private:

        MRVLOMXInterface()
        {
	    ipHandle = NULL;
	    ipHandle = dlopen(OMX_CORE_WRAPPER_LIBRARY, RTLD_NOW);
	    if (NULL == ipHandle)
	    {
		LOGE("Error opening %s: (%s). Will try opening OMX core lib directly", OMX_CORE_WRAPPER_LIBRARY, dlerror());
                ipHandle = dlopen(OMX_CORE_LIBRARY, RTLD_NOW);
	    }
	    else
	    {
		LOGD("loading %s succeeded", OMX_CORE_WRAPPER_LIBRARY);
	    }


            if (NULL == ipHandle)
            {
                pOMX_Init = NULL;
                pOMX_Deinit = NULL;
                pOMX_ComponentNameEnum = NULL;
                pOMX_GetHandle = NULL;
                pOMX_FreeHandle = NULL;
                pOMX_GetComponentsOfRole = NULL;
                pOMX_GetRolesOfComponent = NULL;
                pOMX_SetupTunnel = NULL;
                pOMX_GetContentPipe = NULL;

                // check for errors
                const char* pErr = dlerror();
                if (NULL == pErr)
                {
                    // No error reported, but no handle to the library
                    LOGE("OsclLib::LoadLibrary: Error opening "
                         "library (%s) but no error reported\n", OMX_CORE_LIBRARY);
                }
                else
                {
                    // Error reported
                    LOGE("OsclLib::LoadLibrary: Error opening "
                         "library (%s): %s\n", OMX_CORE_LIBRARY, pErr);
                }
            }
            else
            {
                // Lookup all the symbols in the OMX core
                pOMX_Init = (tpOMX_Init)dlsym(ipHandle, "OMX_Init");
		LOGD("OMX_Init %p", pOMX_Init);
                pOMX_Deinit = (tpOMX_Deinit)dlsym(ipHandle, "OMX_Deinit");
		LOGD("OMX_Deinit %p", pOMX_Deinit);
                pOMX_ComponentNameEnum = (tpOMX_ComponentNameEnum)dlsym(ipHandle, "OMX_ComponentNameEnum");
		LOGD("OMX_ComponentNameEnum %p", pOMX_ComponentNameEnum);
                pOMX_GetHandle = (tpOMX_GetHandle)dlsym(ipHandle, "OMX_GetHandle");
		LOGD("OMX_GetHandle %p", pOMX_GetHandle);
                pOMX_FreeHandle = (tpOMX_FreeHandle)dlsym(ipHandle, "OMX_FreeHandle");
		LOGD("OMX_FreeHandle %p", pOMX_FreeHandle);
                pOMX_GetComponentsOfRole = (tpOMX_GetComponentsOfRole)dlsym(ipHandle, "OMX_GetComponentsOfRole");
		LOGD("OMX_GetComponentsOfRole %p", pOMX_GetComponentsOfRole);
                pOMX_GetRolesOfComponent = (tpOMX_GetRolesOfComponent)dlsym(ipHandle, "OMX_GetRolesOfComponent");
		LOGD("OMX_GetRolesOfComponent %p", pOMX_GetRolesOfComponent);
                pOMX_SetupTunnel = (tpOMX_SetupTunnel)dlsym(ipHandle, "OMX_SetupTunnel");
		LOGD("OMX_SetupTunnel %p", pOMX_SetupTunnel);
                pOMX_GetContentPipe = (tpOMX_GetContentPipe)dlsym(ipHandle, "OMX_GetContentPipe");
		LOGD("OMX_GetContentPipe %p", pOMX_GetContentPipe);

	        ipConfigHandle = NULL;
	        ipConfigHandle = dlopen(OMX_CONFIG_LIBRARY, RTLD_NOW);
		if (NULL == ipConfigHandle)
		{
		    const char* pErr = dlerror();
		    if (NULL == pErr)
		    {
			LOGE("OsclLib::LoadLibrary: Error opening "
                             "library (%s) but no error reported\n", OMX_CONFIG_LIBRARY);
		    }
		    else
		    {
                    	LOGE("OsclLib::LoadLibrary: Error opening "
                             "library (%s): %s\n", OMX_CONFIG_LIBRARY, pErr);
		    }
		}

                pOMXConfigParser = (tpOMXConfigParser)dlsym(ipConfigHandle, "MRVLOMXConfigParser");
		if (pOMXConfigParser == NULL) {
		    LOGD("OMXConfigParser is not defined. Use native parser instead.");
		} else {
		    LOGD("Using MRVLOMXConfigParser.");
		}
            }
        };

};

// function to obtain the interface object from the shared library
extern "C"
{
    OSCL_EXPORT_REF OsclAny* PVGetInterface()
    {
        return MRVLOMXInterface::Instance();
    }
    OSCL_EXPORT_REF void PVReleaseInterface(void* interface)
    {
	MRVLOMXInterface * pInterface = (MRVLOMXInterface*)interface;
	if (pInterface)
	{
		OSCL_DELETE(pInterface);
	}
    }
}

