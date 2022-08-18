/*
 * wd_int_mpsm.c
 *
 *  Created on: Aug 17, 2022
 *      Author: iris
 */

#include "comms/wd_int_mpsm.h"

//###########################################################
// Private types
//###########################################################

typedef enum WdIntMpsm__State {
    //TODO
    WDINTMPSM__STATE__PLACEHOLDER
} WdIntMpsm__State;

struct WdIntMpsm__State
{
    WdIntMpsm__State state;
};

//###########################################################
// Private globals and constants
//###########################################################

static WdIntMpsm__State theState = {
    .state = WDINTMPSM__STATE__PLACEHOLDER
};

//###########################################################
// Public function definitions
//###########################################################

WdIntMpsm__Status WdIntMpsm__processEdge(BOOL edgeWasRising, uint16_t flatDuration)
{
    return WD_INT_MPSM__STATUS__NEED_MORE_DATA;
}
