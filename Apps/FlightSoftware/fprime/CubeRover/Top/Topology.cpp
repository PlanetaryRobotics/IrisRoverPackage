#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>

#include "CubeRoverConfig.hpp"
#include "Topology.hpp"
#include "Components.hpp"


Drv::BlockDriverImpl blockDriver;

static NATIVE_INT_TYPE rgDivs[] = {RATEGROUP_DIVIDER_HI_FREQ,
                                   RATEGROUP_DIVIDER_MED_FREQ,
                                   RATEGROUP_DIVIDER_LOW_FREQ};
// Rate Group Driver Component
Svc::RateGroupDriverImpl rateGroupDriver(
#if FW_OBJECT_NAMES == 1
    "RateGroupDriver",
#endif
     rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

// Rate group - Low Frequency tasks
static NATIVE_UINT_TYPE rgLoFreqContext[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupLowFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupLowFreq", 
#endif
    rgLoFreqContext, FW_NUM_ARRAY_ELEMENTS(rgLoFreqContext));

// Rate group - Medium Frequency tasks
static NATIVE_UINT_TYPE rgMedFreqContext[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupMedFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupMedFreq",
#endif
    rgMedFreqContext, FW_NUM_ARRAY_ELEMENTS(rgMedFreqContext));

// Rate gtoup - High Frequency tasks
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

    // Initialize rate group driver driver (passive)
    rateGroupDriverComp.init();

    // Initialize rate group components (active)
    rateGroupLowFreq.init(RG_LOW_FREQ_ID, RG_LOW_FREQ_QUEUE_DEPTH);
    rateGroupMedFreq.init(RG_MED_FREQ_ID, RG_MED_FREQ_QUEUE_DEPTH);
    rateGroupHiFreq.init(RG_HI_FREQ_ID, RG_HI_FREQ_QUEUE_DEPTH);

	constructCubeRoverArchitecture();
}
