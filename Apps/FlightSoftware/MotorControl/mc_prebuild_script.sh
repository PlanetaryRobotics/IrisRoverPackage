#!/bin/bash

MotorControl_path=$(dirname $0)
SharedLibs_path=$(dirname $0)/../../../Libs/FlightSoftware/MotorControl

rm -rf $MotorControl_path/ICD_*
cp -r $SharedLibs_path/ICD_* $MotorControl_path/
