#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>

#include "CubeRoverConfig.hpp"
#include "Topology.hpp"
#include "Components.hpp"

// ---------------------------------------------------------------------------
// Block Driver Component
Drv::BlockDriverImpl blockDriver(
#if FW_OBJECT_NAMES == 1
    "BlockDriver"
#endif
);

// ---------------------------------------------------------------------------
// Rate Group Driver Component
static NATIVE_INT_TYPE rgDivs[] = {RATEGROUP_DIVIDER_HI_FREQ,
                                   RATEGROUP_DIVIDER_MED_FREQ,
                                   RATEGROUP_DIVIDER_LOW_FREQ};
Svc::RateGroupDriverImpl rateGroupDriver(
#if FW_OBJECT_NAMES == 1
    "RateGroupDriver",
#endif
     rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

// ---------------------------------------------------------------------------
// Rate group - Low Frequency tasks
static NATIVE_UINT_TYPE rgLoFreqContext[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupLowFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupLowFreq", 
#endif
    rgLoFreqContext, FW_NUM_ARRAY_ELEMENTS(rgLoFreqContext));

// ---------------------------------------------------------------------------
// Rate group - Medium Frequency tasks
static NATIVE_UINT_TYPE rgMedFreqContext[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupMedFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupMedFreq",
#endif
    rgMedFreqContext, FW_NUM_ARRAY_ELEMENTS(rgMedFreqContext));

// ---------------------------------------------------------------------------
// Rate group - High Frequency tasks
static NATIVE_UINT_TYPE rgHiFreqContext[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupHiFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupHiFreq",
#endif
    rgHiFreqContext, FW_NUM_ARRAY_ELEMENTS(rgHiFreqContext));


void run1cycle(void) {

}


/**
 * @brief      Construct the F-prime application
 */
void constructApp(void){
    //Initialize the block driver
    blockDriver.init(BLK_DRV_QUEUE_DEPTH);

    // Initialize rate group driver driver (passive)
    rateGroupDriver.init();

    // Initialize rate group components (active)
    rateGroupLowFreq.init(RG_LOW_FREQ_QUEUE_DEPTH, RG_LOW_FREQ_ID);
    rateGroupMedFreq.init(RG_MED_FREQ_QUEUE_DEPTH, RG_MED_FREQ_ID);
    rateGroupHiFreq.init(RG_HI_FREQ_QUEUE_DEPTH, RG_HI_FREQ_ID);

    // Construct the application and make all connections between components
	constructCubeRoverArchitecture();

	rateGroupLowFreq.start(0, /* identifier */
	                       RG_LOW_FREQ_AFF, /* Thread affinity */
	                       RG_LOW_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    rateGroupMedFreq.start(0, /* identifier */
                           RG_MED_FREQ_AFF, /* Thread affinity */
                           RG_MED_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    rateGroupHiFreq.start(0, /* identifier */
                           RG_HI_FREQ_AFF, /* Thread affinity */
                           RG_HI_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    blockDriver.start(0, /* identifier */
                     BLK_DRV_AFF, /* Thread affinity */
                     BLK_DRV_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */
}
