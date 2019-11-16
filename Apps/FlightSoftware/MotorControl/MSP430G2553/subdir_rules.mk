################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmsp -O3 --use_hw_mpy=none --include_path="/Applications/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="/Users/zCoCo/Documents/CR-MotorDrivers/MotorController" --include_path="/Users/zCoCo/Documents/CR-MotorDrivers/MotorController/smbuslib" --include_path="/Users/zCoCo/Documents/CR-MotorDrivers/MotorController/smbuslib/MSP430G2xx3" --include_path="/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --advice:power="all" --advice:power_severity=suppress --define=__MSP430G2553__ --define=MCLK_MHZ=8 --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


