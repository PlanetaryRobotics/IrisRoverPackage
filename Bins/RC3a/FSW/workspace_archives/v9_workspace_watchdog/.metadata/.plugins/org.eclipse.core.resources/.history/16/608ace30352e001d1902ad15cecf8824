################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./src/comms/cmd_msgs.obj" \
"./src/comms/debug_comms.obj" \
"./src/comms/ground_msgs.obj" \
"./src/comms/hercules_comms.obj" \
"./src/comms/hercules_mpsm.obj" \
"./src/comms/hercules_msgs.obj" \
"./src/comms/i2c_sensors.obj" \
"./src/comms/ip_udp.obj" \
"./src/comms/lander_comms.obj" \
"./src/comms/slip_encode.obj" \
"./src/comms/slip_mpsm.obj" \
"./src/comms/watchdog_cmd_msgs.obj" \
"./src/comms/wd_int_mpsm.obj" \
"./src/drivers/adc.obj" \
"./src/drivers/blimp.obj" \
"./src/drivers/bsp.obj" \
"./src/drivers/i2c.obj" \
"./src/drivers/uart.obj" \
"./src/event/event_queue.obj" \
"./src/main.obj" \
"./src/watchdog.obj" \
"./src/stateMachine/RoverState.obj" \
"./src/stateMachine/RoverStateBase.obj" \
"./src/stateMachine/RoverStateEnteringKeepAlive.obj" \
"./src/stateMachine/RoverStateEnteringMission.obj" \
"./src/stateMachine/RoverStateEnteringService.obj" \
"./src/stateMachine/RoverStateEnteringStasis.obj" \
"./src/stateMachine/RoverStateInit.obj" \
"./src/stateMachine/RoverStateKeepAlive.obj" \
"./src/stateMachine/RoverStateManager.obj" \
"./src/stateMachine/RoverStateMission.obj" \
"./src/stateMachine/RoverStateService.obj" \
"./src/stateMachine/RoverStateStasis.obj" \
"./src/utils/ring_buffer.obj" \
"./src/utils/serialization.obj" \
"./src/utils/time.obj" \
"../lnk_msp430fr5994.cmd" \
$(GEN_CMDS__FLAG) \
-llibmpu_init.a \
-llibmath.a \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/comms/subdir_vars.mk
-include src/drivers/subdir_vars.mk
-include src/event/subdir_vars.mk
-include src/subdir_vars.mk
-include src/stateMachine/subdir_vars.mk
-include src/utils/subdir_vars.mk
-include subdir_rules.mk
-include src/comms/subdir_rules.mk
-include src/drivers/subdir_rules.mk
-include src/event/subdir_rules.mk
-include src/subdir_rules.mk
-include src/stateMachine/subdir_rules.mk
-include src/utils/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Watchdog.out \

EXE_OUTPUTS__QUOTED += \
"Watchdog.out" \

BIN_OUTPUTS += \
Watchdog.hex \

BIN_OUTPUTS__QUOTED += \
"Watchdog.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Watchdog.out"

# Tool invocations
Watchdog.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: MSP430 Linker'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --advice:power="all" --advice:hw_config="all" --define=__MSP430FR5994__ --define=ENABLE_DEBUG_ONLY_CODE --define=_MPU_ENABLE --define=RADIO_PROGRAMMING_MODE -g --c11 --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --emit_warnings_as_errors --verbose_diagnostics --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --entry_hook=hook_sp_check --entry_parm=name -z -m"Watchdog.map" --heap_size=4 --stack_size=1492 --cinit_hold_wdt=on -i"/home/iris/ti/ccs910/ccs/ccs_base/msp430/include" -i"/home/iris/ti/ccs910/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx" -i"/home/iris/ti/ccs910/ccs/ccs_base/msp430/lib/FR59xx" -i"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/lib" -i"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --priority --reread_libs --define=_MPU_ENABLE --diag_wrap=off --display_error_number --warn_sections --xml_link_info="Watchdog_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "Watchdog.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

Watchdog.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: MSP430 Hex Utility'
	"/home/iris/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/hex430" --memwidth=8 --romwidth=8 -o "Watchdog.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "src/comms/cmd_msgs.obj" "src/comms/debug_comms.obj" "src/comms/ground_msgs.obj" "src/comms/hercules_comms.obj" "src/comms/hercules_mpsm.obj" "src/comms/hercules_msgs.obj" "src/comms/i2c_sensors.obj" "src/comms/ip_udp.obj" "src/comms/lander_comms.obj" "src/comms/slip_encode.obj" "src/comms/slip_mpsm.obj" "src/comms/watchdog_cmd_msgs.obj" "src/comms/wd_int_mpsm.obj" "src/drivers/adc.obj" "src/drivers/blimp.obj" "src/drivers/bsp.obj" "src/drivers/i2c.obj" "src/drivers/uart.obj" "src/event/event_queue.obj" "src/main.obj" "src/watchdog.obj" "src/stateMachine/RoverState.obj" "src/stateMachine/RoverStateBase.obj" "src/stateMachine/RoverStateEnteringKeepAlive.obj" "src/stateMachine/RoverStateEnteringMission.obj" "src/stateMachine/RoverStateEnteringService.obj" "src/stateMachine/RoverStateEnteringStasis.obj" "src/stateMachine/RoverStateInit.obj" "src/stateMachine/RoverStateKeepAlive.obj" "src/stateMachine/RoverStateManager.obj" "src/stateMachine/RoverStateMission.obj" "src/stateMachine/RoverStateService.obj" 
	-$(RM) "src/stateMachine/RoverStateStasis.obj" "src/utils/ring_buffer.obj" "src/utils/serialization.obj" "src/utils/time.obj" 
	-$(RM) "src/comms/cmd_msgs.d" "src/comms/debug_comms.d" "src/comms/ground_msgs.d" "src/comms/hercules_comms.d" "src/comms/hercules_mpsm.d" "src/comms/hercules_msgs.d" "src/comms/i2c_sensors.d" "src/comms/ip_udp.d" "src/comms/lander_comms.d" "src/comms/slip_encode.d" "src/comms/slip_mpsm.d" "src/comms/watchdog_cmd_msgs.d" "src/comms/wd_int_mpsm.d" "src/drivers/adc.d" "src/drivers/blimp.d" "src/drivers/bsp.d" "src/drivers/i2c.d" "src/drivers/uart.d" "src/event/event_queue.d" "src/watchdog.d" "src/utils/ring_buffer.d" "src/utils/serialization.d" "src/utils/time.d" 
	-$(RM) "src/main.d" "src/stateMachine/RoverState.d" "src/stateMachine/RoverStateBase.d" "src/stateMachine/RoverStateEnteringKeepAlive.d" "src/stateMachine/RoverStateEnteringMission.d" "src/stateMachine/RoverStateEnteringService.d" "src/stateMachine/RoverStateEnteringStasis.d" "src/stateMachine/RoverStateInit.d" "src/stateMachine/RoverStateKeepAlive.d" "src/stateMachine/RoverStateManager.d" "src/stateMachine/RoverStateMission.d" "src/stateMachine/RoverStateService.d" "src/stateMachine/RoverStateStasis.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

