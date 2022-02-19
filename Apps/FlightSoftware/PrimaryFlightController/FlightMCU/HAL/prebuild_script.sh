#!/bin/bash

FlightMCU_path=$(dirname $0)


### Patch HAL's rti.c by appending a C preprocessor #endif since there is no user code section
rti_c_path=$FlightMCU_path/HAL/source/rti.c
tail -n1 $rti_c_path | grep -q "#endif"
patch_rti_c=$?

if [ $patch_rti_c -ne 0 ]
then
    echo "Patching $rti_c_path"
    echo "#endif" >> $rti_c_path
fi


### Patch HAL's adc.c with a byte in the adcSelect which allows our thermistors to work
patch --quiet $FlightMCU_path/HAL/source/adc.c $FlightMCU_path/patches/adc.c.patch
