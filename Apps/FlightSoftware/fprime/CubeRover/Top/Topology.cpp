#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>

#include "CubeRoverConfig.hpp"
#include "Topology.hpp"
#include "Components.hpp"

// ---------------------------------------------------------------------------
// Block Driver Component
// Block driver generates trigger signal for rate group driver
Drv::BlockDriverImpl blockDriver(
#if FW_OBJECT_NAMES == 1
    "BlockDriver"
#endif
);

// ---------------------------------------------------------------------------
// Rate Group Driver Component
// That array sets the frequency divider for the rate groups
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
static NATIVE_UINT_TYPE rgLoFreqContext[] = {0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupLowFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupLowFreq", 
#endif
    rgLoFreqContext, FW_NUM_ARRAY_ELEMENTS(rgLoFreqContext));

// ---------------------------------------------------------------------------
// Rate group - Medium Frequency tasks
static NATIVE_UINT_TYPE rgMedFreqContext[] = {0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupMedFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupMedFreq",
#endif
    rgMedFreqContext, FW_NUM_ARRAY_ELEMENTS(rgMedFreqContext));

// ---------------------------------------------------------------------------
// Rate group - High Frequency tasks
static NATIVE_UINT_TYPE rgHiFreqContext[] = {0,0,0,0};
Svc::ActiveRateGroupImpl rateGroupHiFreq(
#if FW_OBJECT_NAMES == 1
    "RateGroupHiFreq",
#endif
    rgHiFreqContext, FW_NUM_ARRAY_ELEMENTS(rgHiFreqContext));

// ---------------------------------------------------------------------------
// Time - contains current CubeRover Time used for time stamping events
Svc::CubeRoverTimeImpl cubeRoverTime(
#if FW_OBJECT_NAMES == 1
    "CubeRoverTime"
#endif
     );

// ---------------------------------------------------------------------------
// Telemetric channel component used to centralized of telemetric data
Svc::TlmChanImpl tlmChan(
#if FW_OBJECT_NAMES == 1
    "TlmChan"
#endif
    );

// ---------------------------------------------------------------------------
// Active logger data component used to log system events
Svc::ActiveLoggerImpl activeLogger(
#if FW_OBJECT_NAMES == 1
        "ActiveLogger"
#endif
    );

// ---------------------------------------------------------------------------
// Active logger data component used to log system events
CubeRover::MotorControlComponentImpl motorControl(
#if FW_OBJECT_NAMES == 1
        "MotorControl"
#endif
);

// ---------------------------------------------------------------------------
// Serial interface to support radio interface
Drv::FreeRtosSerialDriverComponentImpl radioSerialInterface(
#if FW_OBJECT_NAMES == 1
        "RadioInterface"
#endif
);

// Temporary code to simulate block driver rate group
void run1cycle(void) {
    blockDriver.callIsr();
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

    // Initialize cubeRover time component (passive)
    cubeRoverTime.init(0);

    // Initialize the telemetric channel component (active)
    tlmChan.init(TLM_CHAN_QUEUE_DEPTH, TLM_CHAN_ID);

    // Initialize the active logger
    activeLogger.init(ACTIVE_LOGGER_QUEUE_DEPTH, ACTIVE_LOGGER_ID);

    // Initialize serial interface to radio module
    radioSerialInterface.init();

    // Initialize the motor control
    motorControl.init(MOTOR_CONTROL_QUEUE_DEPTH, MOTOR_CONTROL_ID);

    // Construct the application and make all connections between components
	constructCubeRoverArchitecture();

	rateGroupLowFreq.start(RG_LOW_FREQ_ID, /* identifier */
	                       RG_LOW_FREQ_AFF, /* CPU priority */
	                       RG_LOW_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    rateGroupMedFreq.start(RG_MED_FREQ_ID, /* identifier */
                           RG_MED_FREQ_AFF, /* CPU priority  */
                           RG_MED_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    rateGroupHiFreq.start(RG_HI_FREQ_ID, /* identifier */
                          RG_HI_FREQ_AFF, /* CPU priority  */
                          RG_HI_FREQ_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    blockDriver.start(BLK_DRV_ID, /* identifier */
                      BLK_DRV_AFF, /* CPU priority  */
                      BLK_DRV_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    tlmChan.start(TLM_CHAN_ID, /* identifier */
                  TLM_CHAN_AFF, /* CPU priority */
                  TLM_CHAN_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    activeLogger.start(ACTIVE_LOGGER_ID, /* identifier */
                       ACTIVE_LOGGER_AFF, /* CPU priority  */
                       ACTIVE_LOGGER_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    motorControl.start(MOTOR_CONTROL_ID, /* identifier */
                       MOTOR_CONTROL_AFF, /* CPU priority  */
                       MOTOR_CONTROL_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES); /* stack size */

    radioSerialInterface.open(sciREG,
                              Drv::FreeRtosSerialDriverComponentImpl::BAUD_9600,
                              Drv::FreeRtosSerialDriverComponentImpl::NO_FLOW,
                              Drv::FreeRtosSerialDriverComponentImpl::PARITY_NONE,
                              false);

    radioSerialInterface.startReadThread(RADIO_SERIAL_AFF, RADIO_SERIAL_QUEUE_DEPTH*MIN_STACK_SIZE_BYTES);
}
