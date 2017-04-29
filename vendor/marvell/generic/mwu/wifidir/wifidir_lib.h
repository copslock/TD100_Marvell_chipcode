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

#ifndef __WIFIDIR_LIB_H__
#define __WIFIDIR_LIB_H__

#include "mwu_defs.h"
#include "wifidir.h"

/**
 *  Performs the ioctl operation to send the command to the driver.
 *
 * params
 *     ifname        A pointer to net name
 *     cmd           Pointer to the command buffer
 *     size          Pointer to the command size. This value is overwritten
 *                   by the function with the size of the received response.
 *     buf_size      Size of the allocated command buffer
 *
 * return            WIFIDIR_ERR_SUCCESS--success, WIFIDIR_ERR_COM--fail
 */
int wifidir_ioctl(char *ifname, u8 * cmd, u16 * size, u16 buf_size);

#endif
