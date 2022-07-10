#!/bin/bash

# Creates a temporary CCS workspace (from a clean-slate) and adds this project
# to it. Remove it using `ccs_temp_workspace__destroy.sh`.

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory

# Destroy it in case it already exists:
./ccs_temp_workspace__destroy.sh

# Create it implicity by referencing it and linking it to the project:
$TI_CCS_EXECUTABLE \
    -noSplash \
    -data $FlightMCU_path \
    -application com.ti.ccstudio.apps.projectImport \
    -ccs.location $FlightMCU_path

cd $StartingDir # Return to where we were

