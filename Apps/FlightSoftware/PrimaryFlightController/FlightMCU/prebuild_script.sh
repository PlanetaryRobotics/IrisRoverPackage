#!/bin/bash

FlightMCU_path=$(dirname $0)
FPrime_path=$(dirname $0)/../../fprime/CubeRover
SharedLibs_path=$(dirname $0)/../../../../Libs/FlightSoftware

rm -rf $FPrime_path/MotorControl/ICD_*
cp -r $SharedLibs_path/MotorControl/ICD_* $FPrime_path/MotorControl/

### Patch HAL's rti.c by appending a C preprocessor #endif since there is no user code section
rti_c_path=$FlightMCU_path/HAL/source/rti.c
tail -n1 $rti_c_path | grep -q "#endif"
patch_rti_c=$?

if [ $patch_rti_c -ne 0 ]
then
    echo "Patching $rti_c_path"
    echo "#endif" >> $rti_c_path
fi

