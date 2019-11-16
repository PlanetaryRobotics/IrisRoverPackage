################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./Driver_HAL.obj" \
"./HAL.obj" \
"./main.obj" \
"./smbuslib/MSP430G2xx3/smbus_phy.obj" \
"./smbuslib/smbus.obj" \
"./smbuslib/smbus_nwk.obj" \
"../lnk_msp430g2553.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include smbuslib/MSP430G2xx3/subdir_vars.mk
-include smbuslib/subdir_vars.mk
-include subdir_rules.mk
-include smbuslib/MSP430G2xx3/subdir_rules.mk
-include smbuslib/subdir_rules.mk
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
MotorController.out \

EXE_OUTPUTS__QUOTED += \
"MotorController.out" \

BIN_OUTPUTS += \
MotorController.hex \

BIN_OUTPUTS__QUOTED += \
"MotorController.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "MotorController.out"

# Tool invocations
MotorController.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: MSP430 Linker'
	"/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmsp -O3 --use_hw_mpy=none --advice:power="all" --advice:power_severity=suppress --define=__MSP430G2553__ --define=MCLK_MHZ=8 --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number -z -m"MotorController.map" --heap_size=80 --stack_size=80 -i"/Applications/ti/ccs920/ccs/ccs_base/msp430/include" -i"/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/lib" -i"/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="MotorController_linkInfo.xml" --use_hw_mpy=none --rom_model -o "MotorController.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

MotorController.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: MSP430 Hex Utility'
	"/Applications/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/hex430" --memwidth=8 --romwidth=8 -o "MotorController.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "Driver_HAL.obj" "HAL.obj" "main.obj" "smbuslib/MSP430G2xx3/smbus_phy.obj" "smbuslib/smbus.obj" "smbuslib/smbus_nwk.obj" 
	-$(RM) "Driver_HAL.d" "HAL.d" "main.d" "smbuslib/MSP430G2xx3/smbus_phy.d" "smbuslib/smbus.d" "smbuslib/smbus_nwk.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

