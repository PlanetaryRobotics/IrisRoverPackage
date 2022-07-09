#!/bin/bash

StartingDir=$PWD
FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory

echo "PWD @ $StartingDir"
echo "CCS @ $TI_CCS_INSTALL_DIR"
echo "TODO."

cd $StartingDir # Return to where we were