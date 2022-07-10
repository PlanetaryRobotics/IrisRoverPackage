#!/bin/bash

# Cleans the CCS Project (removes all built files: .hex, .out, .obj, etc.).

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory
./scripts/ccs_temp_workspace__create.sh

$TI_CCS_EXECUTABLE \
    -noSplash \
    -data $TI_CCS_TEMP_WORKSPACE \
    -application com.ti.ccstudio.apps.projectBuild \
    -ccs.projects FlightMCU \
    -ccs.clean

./scripts/ccs_temp_workspace__destroy.sh
cd $StartingDir # Return to where we were