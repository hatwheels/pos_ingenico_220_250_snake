/*
 * gamedev.h
 *
 *  Created on: Mar 10, 2017
 *      Author: hatwheels
 */

#if !defined(__GAMEDEV_H__)
#define __GAMEDEV_H__

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "OSL_CommonTypes.h"
#include "OSL_Security.h"
#include "OSL_SharedMem.h"
#include "oem_public.h"
#include <libgr.h>
#include "os.h"

#include "inc/devices.h"
#include "inc/udisplay.h"
#include "inc/menu.h"

/// Pixel coordinates min/max range
#define Xmin 0
#define Xmax 127
#define Ymin 0
#define Ymax 63

typedef struct blob_t {
    int xs, ys, xe, ye;
} blob;

#endif /* __GAMEDEV_H__ */
