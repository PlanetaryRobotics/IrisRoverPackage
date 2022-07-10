#!/bin/bash

# Cleans the CCS Project (removes all built files: .hex, .out, .obj, etc.).

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

./ccs_temp_workspace__create.sh
cd $FlightMCU_path # enter FlightMCU directory

$TI_CCS_EXECUTABLE \
    -noSplash \
    -data $TI_CCS_TEMP_WORKSPACE \
    -application com.ti.ccstudio.apps.projectBuild \
    -ccs.projects FlightMCU \
    -ccs.clean

cd $StartingDir # Return to where we were
./ccs_temp_workspace__destroy.sh