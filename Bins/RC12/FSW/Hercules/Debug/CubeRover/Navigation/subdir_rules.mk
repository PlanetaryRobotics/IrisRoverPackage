################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CubeRover/Navigation/NavigationComponent.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/Navigation/NavigationComponent.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include/Shared_Libs/MotorControl" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" --define=FPRIME_BUILD -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/Navigation/$(basename $(<F)).d_raw" --obj_directory="CubeRover/Navigation" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

CubeRover/Navigation/NavigationComponentAc.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/Navigation/NavigationComponentAc.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include/Shared_Libs/MotorControl" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" --define=FPRIME_BUILD -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/Navigation/$(basename $(<F)).d_raw" --obj_directory="CubeRover/Navigation" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

