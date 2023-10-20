#!/bin/bash

MotorControl_path=$(dirname $0)
SharedLibs_path=$(dirname $0)/../../../Libs/FlightSoftware/MotorControl

rm -rf $MotorControl_path/Shared_Libs
mkdir -p $MotorControl_path/Shared_Libs
cp -r $SharedLibs_path/* $MotorControl_path/Shared_Libs
