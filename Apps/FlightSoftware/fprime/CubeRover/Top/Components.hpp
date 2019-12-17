/*
 * Components.hpp
 *
 *  Created on: Oct 2, 2019
 *      Author: cedric
 */

#ifndef F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_
#define F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_

// Include drivers
#include <Drv/BlockDriver/BlockDriverImpl.hpp>

// Include services
#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroupImpl.hpp>
#include <Svc/LinuxTime/CubeRoverTimeImpl.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>

// Include component exclusive to CubeRover project
#include "IMU/ImuComponentImpl.hpp"

// Block driver sequencing F-Prime and other sync events
extern Drv::BlockDriverImpl blockDriver;

// Components dealing with Synchronizing F-Prime framework
extern Svc::RateGroupDriverImpl rateGroupDriver;
extern Svc::ActiveRateGroupImpl rateGroupMedFreq;
extern Svc::ActiveRateGroupImpl rateGroupLowFreq;
extern Svc::ActiveRateGroupImpl rateGroupHiFreq;

// Time component that keep track of time on CubeRover
extern Svc::CubeRoverTimeImpl cubeRoverTime;

// Telemetric channel component that takes care of channeling all data to a central place
extern Svc::TlmChanImpl tlmChan;

// Service that handle active logging
extern Svc::ActiveLoggerImpl activeLogger;

// command dispatcher
extern Svc::CommandDispatcherImpl cmdDispatcher;

// Components exclusive to CubeRover
extern CubeRover::ImuComponentImpl imu;

#endif /* F_PRIME_CUBEROVER_TOP_COMPONENTS_HPP_ */
