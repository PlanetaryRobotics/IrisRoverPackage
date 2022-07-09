#!/bin/bash

# Automatically dispatched to by CCS build process.
# This is set in ../.cproject at xpath:
# /cproject
# /storageModule[@moduleId="org.eclipse.cdt.core.settings"]
# /cconfiguration[@id="com.ti.ccstudio.buildDefinitions.TMS470.Debug.284604706"]
# /storageModule[@moduleId="cdtBuildSystem"]
# /configuration/@postbuildStep
cd .. # move into the FlightMCU dir

FlightMCU_path=$(dirname $0)

cat $FlightMCU_path/Include/Version.h
