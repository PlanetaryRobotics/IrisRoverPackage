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

typedef struct WdIntMpsm__TheState
{
    WdIntMpsm__State state;
} WdIntMpsm__TheState;

//###########################################################
// Private globals and constants
//###########################################################

static WdIntMpsm__TheState theState = {
    .state = WDINTMPSM__STATE__PLACEHOLDER
};

//###########################################################
// Public function definitions
//###########################################################

WdIntMpsm__Status WdIntMpsm__processEdge(BOOL edgeWasRising, uint16_t flatDuration)
{
    (void) theState;

    // If the edge is rising we necessarily have WiFi:
    // (once we start actually implementing a messaging protocol, this logic may
    // want to be more advanced since that's not the only thing that could be meant
    // by a rising edge, though it *always* will necessarily mean we have WiFi
    // (Radio will only message us when it has WiFi):
    if(edgeWasRising){
        return WD_INT_MPSM__STATUS__PARSED_GOT_WIFI;
    }

    // TODO: Need to ideally implement the opposite that looks for a falling edge and
    // then no more edges for X time = DISCONNECTED (i.e. Radio holds LOW and stops
    // messaging when DISCONNECTED). For now though, the big thing we care about is
    // that the WD can know when the Radio connects (so it can stop implementing the
    // crawl forward stuff).

    return WD_INT_MPSM__STATUS__NEED_MORE_DATA;
}
