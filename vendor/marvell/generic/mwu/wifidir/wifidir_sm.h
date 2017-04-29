/*
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
/* wifidir_sm.h: Internal API for calling the wifidirect state machine
 *
 * The wifidirect state machine handles wifidirect events that come from the
 * driver.  This driver stream emits events of many types.  We should be able
 * to register our state machine as a handler for events of type
 * EV_ID_WIFIDIR_GENERIC, but that API doesn't exist yet.  So we call the state
 * machine explicitly.  For this, we need a proper header file.
 */

#ifndef __WIFIDIR_SM_H__
#define __WIFIDIR_SM_H__

#include "wps_def.h"

/*
 * @brief process WIFIDIR events and advance the state machine
 *
 * This is the main WIFIDIR state machine.  It handles events from different event
 * spaces.  Events from the WIFIDIR_EVENTS_DRIVER space come from the driver and
 * have ID == EV_ID_WIFIDIR_GENERIC.
 *
 * Calls to this function must be serialized.  At this time, we depend on the
 * fact that there is a single event loop to serialize this.  If multiple event
 * sources emerge, some sort of locking will have to be implemented.
 *
 * @param buffer   Pointer to received buffer
 * @param size     Length of the received event data
 * @param es       Name of the event space from which the incoming event
 *                 was gathered.
 * @return         N/A
 */
int wifidir_state_machine(u8 *buffer, u16 size, int es);

enum {
    WIFIDIR_EVENTS_DRIVER,
    WIFIDIR_EVENTS_UI,
};

#endif
