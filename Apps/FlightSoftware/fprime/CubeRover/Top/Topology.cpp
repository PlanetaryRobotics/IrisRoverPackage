#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>

#include "Topology.hpp"
#include "Components.hpp"

static NATIVE_INT_TYPE rgDivs[] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp
#if FW_OBJECT_NAMES == 1
    ("RateGroupDriver",
#endif
     rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

void run1cycle(void) {

}

/**
 * @brief      Contruct application
 */
void constructApp(void){

    // Initialize rate group driver
    rateGroupDriverComp.init();

	constructCubeRoverArchitecture();
}
