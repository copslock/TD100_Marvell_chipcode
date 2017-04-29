/* ------------------------------------------------------------------
 * Copyright (C) 2009 Android Open Source Project
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

#ifndef ANDROID_SURFACE_OUTPUT_3D_H_INCLUDED
#define ANDROID_SURFACE_OUTPUT_3D_H_INCLUDED

#include "android_surface_output.h"

#include "pv_mime_string_utils.h"
#include "pvmf_video.h"
#include "ccyuv422toyuv420.h"


class AndroidSurfaceOutput3D : public AndroidSurfaceOutput
{
public:
    AndroidSurfaceOutput3D();

    // frame buffer interface
    virtual bool initCheck();
    virtual PVMFStatus writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info);
    virtual void postLastFrame();
    virtual void closeFrameBuf();

    OSCL_IMPORT_REF ~AndroidSurfaceOutput3D();

private:
    ColorConvertBase* iYUV422toYUV420PConverter; //for uyvy->420p conversion
    OsclAny* pTempBuf;
};

#endif // ANDROID_SURFACE_OUTPUT_OV_H_INCLUDED
