################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CubeRover/IMU/IMUComponent.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/IMU/IMUComponent.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/IMU/$(basename $(<F)).d_raw" --obj_directory="CubeRover/IMU" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

CubeRover/IMU/IMUComponentAc.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/IMU/IMUComponentAc.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/IMU/$(basename $(<F)).d_raw" --obj_directory="CubeRover/IMU" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

CubeRover/IMU/adxl312.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/IMU/adxl312.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/IMU/$(basename $(<F)).d_raw" --obj_directory="CubeRover/IMU" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

CubeRover/IMU/l3gd20h.obj: /home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover/IMU/l3gd20h.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=0 --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/HAL/include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/PrimaryFlightController/FlightMCU/Include" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/CubeRover" --include_path="/home/iris/Desktop/Working_Folders/mschnur/Watchdog_MSP430/Apps/FlightSoftware/fprime/Fw/Types/TIR4" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="CubeRover/IMU/$(basename $(<F)).d_raw" --obj_directory="CubeRover/IMU" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


