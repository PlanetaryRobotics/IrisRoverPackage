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

CubeRover::PingReceiverComponentImpl pingRcvr
#if FW_OBJECT_NAMES == 1
    ("pingRcvr")
#endif
;


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

Svc::CubeRoverTimeImpl linuxTime
#if FW_OBJECT_NAMES == 1
    ("LNXTM")
#endif
;

static NATIVE_INT_TYPE rgDivs[] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp
#if FW_OBJECT_NAMES == 1
    ("RTGRP",
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

	pingRcvr.init(1);
    mathSender.init(1,0);
    mathReceiver.init(1,0);
    textLogger.init();
    eventLogger.init(EVENT_LOGGER_QUEUE_DEPTH, 0);
    cubeRoverTime.init(0);
    chanTlm.init(TLM_CHAN_QUEUE_DEPTH,0);
    cmdDisp.init(CMD_DISP_QUEUE_DEPTH,0);
    prmDb.init(PARAM_DB_QUEUE_DEPTH,0);
    linuxTime.init(0);

	constructCubeRoverArchitecture();

	// Register commands. Registration order matters.
	mathSender.regCommands();
	mathReceiver.regCommands();

    // start dispatcher
    cmdDisp.start(CMD_DISP_ID, CMD_DISP_AFF, CMD_DISP_QUEUE_DEPTH * 1024);

    // start telemetry
    eventLogger.start(EVENT_LOGGER_ID, EVENT_LOGGER_AFF, EVENT_LOGGER_QUEUE_DEPTH * 1024);
    chanTlm.start(TLM_CHAN_ID, TLM_CHAN_AFF, TLM_CHAN_QUEUE_DEPTH * 1024);
    prmDb.start(PARAM_DB_ID, PARAM_DB_AFF, PARAM_DB_QUEUE_DEPTH * 1024);
}
