################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
driverlib/%.obj: ../driverlib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/iris/ti/ccs1030/ccs/tools/compiler/ti-cgt-msp430_20.2.4.LTS/bin/cl430" -vmspx --data_model=restricted -O2 --use_hw_mpy=F5 --include_path="/home/iris/ti/ccs1030/ccs/ccs_base/msp430/include" --include_path="E:/xprize/Documents/MotorControlDev/MotorControl/MotorControl/driverlib" --include_path="C:/ti/ccs930/ccs/ccs_base/msp430ware_3_80_11_07/iqmathlib/include" --include_path="/home/iris/Desktop/Stable_Testing_Repos/Motor Controllers/Apps/FlightSoftware/MotorControl/driverlib" --include_path="/home/iris/ti/msp430/IQMATHLIB_01_10_00_05/include" --include_path="/home/iris/ti/ccs1030/ccs/tools/compiler/ti-cgt-msp430_20.2.4.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="driverlib/$(basename $(<F)).d_raw" --obj_directory="driverlib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


