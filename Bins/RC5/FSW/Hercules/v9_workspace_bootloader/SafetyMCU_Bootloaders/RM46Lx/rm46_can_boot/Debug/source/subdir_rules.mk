################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
source/Fapi_UserDefinedFunctions.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/Fapi_UserDefinedFunctions.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_check.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_check.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_copySections2SRAM.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_copySections2SRAM.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_dcan.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_dcan.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_flash.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_flash.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_led_demo.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_led_demo.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_main.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_main.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_spi.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_spi.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_spi_packet.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_spi_packet.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_uart.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_uart.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/bl_ymodem.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/bl_ymodem.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/sci_common.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/sci_common.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/sw_hw_ver.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/sw_hw_ver.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

source/ymodem_crc16.obj: /home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/source/ymodem_crc16.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-arm_18.12.2.LTS/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/RM46Lx/rm46_can_boot" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/include" --include_path="/home/iris/ti/Hercules/F021 Flash API/02.01.01/include" --include_path="/home/iris/Desktop/Workspaces/v9_workspace_bootloader/SafetyMCU_Bootloaders/HALCoGen/RM46/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


