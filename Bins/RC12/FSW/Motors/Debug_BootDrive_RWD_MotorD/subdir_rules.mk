################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --data_model=restricted -Ooff --opt_for_speed=0 --use_hw_mpy=F5 --include_path="/home/iris/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/iris/Desktop/Working_folders/RC/Apps/FlightSoftware/MotorControl" --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_folders/RC/Apps/FlightSoftware/MotorControl/driverlib" --include_path="/home/iris/ti/msp430/IQMATHLIB_01_10_00_05/include" --advice:power=all --advice:hw_config="all" --define=__MSP430FR5994__ --define=DRIVE_ON_BOOT --define=DRIVE_ON_BOOT_REAR_DRIVE --define=MOTOR_D --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


