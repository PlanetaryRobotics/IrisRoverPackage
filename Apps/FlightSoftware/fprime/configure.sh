#!/bin/sh
CUBEROVER_PACKAGE_TOP_LEVEL=`git rev-parse --show-toplevel`
cd $CUBEROVER_PACKAGE_TOP_LEVEL/Apps/FlightSoftware/fprime/mk/configs/compiler
echo 'COMMON_DEFINES  := -DISF -DBUILD_$(BUILD)' > defines_common.mk
echo CUBEROVER_PACKAGE_TOP_LEVEL=$CUBEROVER_PACKAGE_TOP_LEVEL >> defines_common.mk
echo FPRIME_ROOT_LOC=$CUBEROVER_PACKAGE_TOP_LEVEL/Apps/FlightSoftware/fprime >> defines_common.mk
