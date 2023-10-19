#!/bin/bash

FlightMCU_path=$(dirname $0)
SharedLibs_path=$(dirname $0)/../../../../Libs/FlightSoftware

rm -rf $FlightMCU_path/Include/Shared_Libs
mkdir -p $FlightMCU_path/Include/Shared_Libs
cp -r $SharedLibs_path/* $FlightMCU_path/Include/Shared_Libs

### Patch HAL's rti.c by appending a C preprocessor #endif since there is no user code section
rti_c_path=$FlightMCU_path/HAL/source/rti.c
tail -n1 $rti_c_path | grep -q "#endif"
patch_rti_c=$?

if [ $patch_rti_c -ne 0 ]
then
    echo "Patching $rti_c_path"
    echo "#endif" >> $rti_c_path
fi

