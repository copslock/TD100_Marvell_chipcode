/***************************************************************************************** 
 * Copyright (c) 2009, Marvell International Ltd. 
 * All Rights Reserved.
 *****************************************************************************************/

#ifndef OMX_MRVL_CODECS_PLUGIN_H_

#define OMX_MRVL_CODECS_PLUGIN_H_

#include <media/stagefright/OMXPluginBase.h>

namespace android {

struct OMXMRVLCodecsPlugin : public OMXPluginBase {
    OMXMRVLCodecsPlugin();
    virtual ~OMXMRVLCodecsPlugin();

    virtual OMX_ERRORTYPE makeComponentInstance(
            const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);

    virtual OMX_ERRORTYPE destroyComponentInstance(
            OMX_COMPONENTTYPE *component);

    virtual OMX_ERRORTYPE enumerateComponents(
            OMX_STRING name,
            size_t size,
            OMX_U32 index);

    virtual OMX_ERRORTYPE getRolesOfComponent(
            const char *name,
            Vector<String8> *roles);

private:
    OMXMRVLCodecsPlugin(const OMXMRVLCodecsPlugin &);
    OMXMRVLCodecsPlugin &operator=(const OMXMRVLCodecsPlugin &);
};

}  // namespace android

#endif  // OMX_MRVL_CODECS_PLUGIN_H_

