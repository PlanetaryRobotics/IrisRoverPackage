#!/bin/bash

# Automatically dispatched to by CCS build process.
# This is set in ../.cproject at xpath:
# /cproject
# /storageModule[@moduleId="org.eclipse.cdt.core.settings"]
# /cconfiguration[@id="com.ti.ccstudio.buildDefinitions.TMS470.Debug.284604706"]
# /storageModule[@moduleId="cdtBuildSystem"]
# /configuration/@prebuildStep

FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory

### Patch HAL's rti.c by appending a C preprocessor #endif since there is no user code section
rti_c_path=$FlightMCU_path/HAL/source/rti.c
tail -n1 $rti_c_path | grep -q "#endif" # look for "#endif" at the end of the file
patch_rti_c=$? # Check: did the last command work?

if [ $patch_rti_c -ne 0 ]
then
    echo "Patching $rti_c_path ..."
    echo "#endif" >> $rti_c_path
else
    echo "Skipping $rti_c_path . Already was patched."
fi


### Patch HAL's adc.c with a byte in the adcSelect which allows our thermistors to work
echo "Patching $FlightMCU_path/HAL/source/adc.c ..."
patch --quiet --forward $FlightMCU_path/HAL/source/adc.c $FlightMCU_path/patches/adc.c.patch

cd - # Return to where we were
