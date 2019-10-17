/*
 * Components.hpp
 *
 *  Created on: Oct 2, 2019
 *      Author: cedric
 */

#ifndef F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_
#define F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_

#include "CubeRover/MathSender/MathSenderComponentImpl.hpp"
#include "CubeRover/MathReceiver/MathReceiverComponentImpl.hpp"
#include "CubeRover/PingReceiver/PingReceiverComponentImpl.hpp"

#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Svc/LinuxTime/CubeRoverTimeImpl.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>
#include <Drv/BlockDriver/BlockDriverImpl.hpp>

extern CubeRover::PingReceiverComponentImpl pingRcvr;
extern CubeRover::MathReceiverComponentImpl mathReceiver;
extern CubeRover::MathSenderComponentImpl mathSender;
extern Svc::ConsoleTextLoggerImpl textLogger;
extern Svc::ActiveLoggerImpl eventLogger;
extern Svc::CubeRoverTimeImpl cubeRoverTime;
extern Svc::TlmChanImpl chanTlm;
extern Svc::CommandDispatcherImpl cmdDisp;
extern Svc::PrmDbImpl prmDb;
extern Svc::RateGroupDriverImpl rateGroupDriverComp;

extern Drv::BlockDriverImpl blockDrv;



#endif /* F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_ */
