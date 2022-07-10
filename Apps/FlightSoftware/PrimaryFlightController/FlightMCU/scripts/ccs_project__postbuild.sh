#!/bin/bash

# Automatically dispatched to by CCS build process.
# This is set in ../.cproject at xpath:
# /cproject
# /storageModule[@moduleId="org.eclipse.cdt.core.settings"]
# /cconfiguration[@id="com.ti.ccstudio.buildDefinitions.TMS470.Debug.284604706"]
# /storageModule[@moduleId="cdtBuildSystem"]
# /configuration/@postbuildStep

FlightMCU_path=$(dirname $0)/.. # FlightMCU is one level up from where this file is saved

cd $FlightMCU_path # enter FlightMCU directory

cat $FlightMCU_path/Include/Version.h

cd - # Return to where we were