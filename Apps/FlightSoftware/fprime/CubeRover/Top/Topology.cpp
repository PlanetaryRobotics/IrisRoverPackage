#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <HAL/include/FreeRTOS.h>

#include "Topology.hpp"
#include "Components.hpp"

CubeRover::MathSenderComponentImpl mathSender
#if FW_OBJECT_NAMES == 1
    ("mathSender")
#endif
;

CubeRover::MathReceiverComponentImpl mathReceiver
#if FW_OBJECT_NAMES == 1
    ("mathReceiver")
#endif
;

//CubeRover::PingReceiverComponentImpl pingRcvr
//#if FW_OBJECT_NAMES == 1
//    ("pingRcvr")
//#endif
//;


Svc::ConsoleTextLoggerImpl textLogger
#if FW_OBJECT_NAMES == 1
    ("textLogger")
#endif
;

Svc::ActiveLoggerImpl eventLogger
#if FW_OBJECT_NAMES == 1
    ("eventLogger")
#endif
;

Svc::CubeRoverTimeImpl cubeRoverTime
#if FW_OBJECT_NAMES == 1
    ("crTime")
#endif
;

Svc::TlmChanImpl chanTlm
#if FW_OBJECT_NAMES == 1
    ("chanTlm")
#endif
;

Svc::CommandDispatcherImpl cmdDisp
#if FW_OBJECT_NAMES == 1
    ("cmdDisp")
#endif
;

Svc::PrmDbImpl prmDb
#if FW_OBJECT_NAMES == 1
    ("prmDb","PrmDb.dat")
#endif
;

// Driver Component
Drv::BlockDriverImpl blockDrv
#if FW_OBJECT_NAMES == 1
    ("BDRV")
#endif
;

static NATIVE_INT_TYPE rgDivs[] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp
#if FW_OBJECT_NAMES == 1
    ("RTGRP",
#endif
     rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

void run1cycle(void) {
    // call interrupt to emulate a clock
    blockDrv.callIsr();
    Os::Task::delay(1000);
}

/**
 * @brief      Contruct application
 */
void constructApp(void){

    // Initialize rate group driver
    rateGroupDriverComp.init();

	//pingRcvr.init(3);
    mathSender.init(1,0);
    mathReceiver.init(1,0);
    textLogger.init();
    eventLogger.init(EVENT_LOGGER_QUEUE_DEPTH, 0);
    cubeRoverTime.init(0);
    chanTlm.init(TLM_CHAN_QUEUE_DEPTH,0);
    cmdDisp.init(CMD_DISP_QUEUE_DEPTH,0);
    prmDb.init(PARAM_DB_QUEUE_DEPTH,0);

    // Initialize block driver
    blockDrv.init(BLOCK_DRV_QUEUE_DEPTH);

	constructCubeRoverArchitecture();

	// Register commands. Registration order matters.
    //pingRcvr.regCommands();
	mathSender.regCommands();
	mathReceiver.regCommands();
    //cmdDisp.regCommands();
	//eventLogger.regCommands();
	//prmDb.regCommands();

	//Start block driver
	blockDrv.start(BLOCK_DRV_ID, BLOCK_DRV_AFF,BLOCK_DRV_QUEUE_DEPTH * 128);

    // start dispatcher
    //cmdDisp.start(CMD_DISP_ID, CMD_DISP_AFF, CMD_DISP_QUEUE_DEPTH * 128);

    // start telemetry
    //eventLogger.start(EVENT_LOGGER_ID, EVENT_LOGGER_AFF, EVENT_LOGGER_QUEUE_DEPTH * 128);
    //chanTlm.start(TLM_CHAN_ID, TLM_CHAN_AFF, TLM_CHAN_QUEUE_DEPTH * 128);
    //prmDb.start(PARAM_DB_ID, PARAM_DB_AFF, PARAM_DB_QUEUE_DEPTH * 128);
}
