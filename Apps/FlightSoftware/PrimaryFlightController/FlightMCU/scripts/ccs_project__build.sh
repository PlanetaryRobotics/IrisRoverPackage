#!/bin/bash

# Builds the Iris source code.
#
# Output binary will be at either "./Debug/FlightMCU.out" or
# "./Release/FlightMCU.out" depending on $TI_CCS_BUILD_TARGET.
# (specifically, it will be at "./$TI_CCS_BUILD_TARGET/FlightMCU.out").

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

./ccs_temp_workspace__create.sh
cd $FlightMCU_path # enter FlightMCU directory

$TI_CCS_EXECUTABLE \
    -noSplash \
    -data $TI_CCS_TEMP_WORKSPACE \
    -application com.ti.ccstudio.apps.projectBuild \
    -ccs.projects FlightMCU \
    -ccs.configuration $TI_CCS_BUILD_TARGET \
    -ccs.buildType full \ # incremental, full, or clean (def. is incremental)
    -ccs.listErrors \
    -ccs.listProblems

cd $StartingDir # Return to where we were
./ccs_temp_workspace__destroy.sh