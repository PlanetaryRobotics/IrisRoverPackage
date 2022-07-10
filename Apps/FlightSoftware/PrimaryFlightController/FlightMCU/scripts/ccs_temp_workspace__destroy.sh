#!/bin/bash

# Cleans up (destroys) a temporary CCS workspace created using
# `ccs_temp_workspace__create.sh`.

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory

# Destroy:
rm -rf $TI_CCS_TEMP_WORKSPACE

cd $StartingDir # Return to where we were


