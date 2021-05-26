#!/bin/bash
# Took variables from mk/configs/compiler/defines_css.mk
TI_ARM_COMPILER_ROOT_LOC=$HOME/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS
TI_ARM_ARCHIVER_ROOT_LOC=$TI_ARM_COMPILER_ROOT_LOC
TI_ARM_COMPILER_BIN_LOC=$TI_ARM_COMPILER_ROOT_LOC/bin/armcl
TI_ARM_COMPILER_INCLUDE_LOC=$TI_ARM_COMPILER_ROOT_LOC/include
TI_ARM_ARCHIVER_BIN_LOC=$TI_ARM_ARCHIVER_ROOT_LOC/bin/armar

pushd littlefs

#-ppd=/home/alec/CubeRoverPackage/Apps/FlightSoftware/fprime/Os/FreeRTOS/tiR4-ti-cgt-arm-bin/lfs_util.d  \
$TI_ARM_COMPILER_BIN_LOC \
	--c99 -DASSERT_FILE_ID=0x2D1B30AD  -ppa \
	-I.. -c \
	-I$TI_ARM_COMPILER_INCLUDE_LOC \
	--output_file lfs.o ./lfs.c 

#-ppd=/home/alec/CubeRoverPackage/Apps/FlightSoftware/fprime/Os/FreeRTOS/tiR4-ti-cgt-arm-bin/lfs_util.d  \
$TI_ARM_COMPILER_BIN_LOC \
	--c99 -DASSERT_FILE_ID=0x2D1B30AD  -ppa \
	-I.. -c \
	-I$TI_ARM_COMPILER_INCLUDE_LOC \
	--output_file lfs_util.o ./lfs_util.c 

$TI_ARM_ARCHIVER_BIN_LOC r ../tiR4-ti-cgt-arm-bin/libOslfs.a ./lfs.o ./lfs_util.o

popd