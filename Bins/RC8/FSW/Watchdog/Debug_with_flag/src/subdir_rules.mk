################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/iris/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/Watchdog" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/Watchdog/include" --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --advice:power="all" --advice:hw_config="all" --define=__MSP430FR5994__ --define=ENABLE_DEBUG_ONLY_CODE --define=_MPU_ENABLE -g --c11 --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --emit_warnings_as_errors --verbose_diagnostics --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --entry_hook=hook_sp_check --entry_parm=name --preproc_with_compile --preproc_dependency="src/$(basename $(<F)).d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/%.obj: ../src/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/iris/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/Watchdog" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/Watchdog/include" --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --advice:power="all" --advice:hw_config="all" --define=__MSP430FR5994__ --define=ENABLE_DEBUG_ONLY_CODE --define=_MPU_ENABLE -g --c11 --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --emit_warnings_as_errors --verbose_diagnostics --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --entry_hook=hook_sp_check --entry_parm=name --preproc_with_compile --preproc_dependency="src/$(basename $(<F)).d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


