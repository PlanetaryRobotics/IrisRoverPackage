################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../HAL/source/sys_link.cmd 

ASM_SRCS += \
../HAL/source/dabort.asm \
../HAL/source/os_portasm.asm \
../HAL/source/sys_core.asm \
../HAL/source/sys_intvecs.asm \
../HAL/source/sys_mpu.asm \
../HAL/source/sys_pmu.asm 

C_SRCS += \
../HAL/source/adc.c \
../HAL/source/crc.c \
../HAL/source/ecap.c \
../HAL/source/emac.c \
../HAL/source/eqep.c \
../HAL/source/errata_SSWF021_45.c \
../HAL/source/esm.c \
../HAL/source/etpwm.c \
../HAL/source/gio.c \
../HAL/source/i2c.c \
../HAL/source/lin.c \
../HAL/source/mdio.c \
../HAL/source/notification.c \
../HAL/source/os_croutine.c \
../HAL/source/os_event_groups.c \
../HAL/source/os_heap.c \
../HAL/source/os_list.c \
../HAL/source/os_mpu_wrappers.c \
../HAL/source/os_port.c \
../HAL/source/os_queue.c \
../HAL/source/os_tasks.c \
../HAL/source/os_timer.c \
../HAL/source/phy_dp83640.c \
../HAL/source/pinmux.c \
../HAL/source/rti.c \
../HAL/source/sci.c \
../HAL/source/spi.c \
../HAL/source/sys_dma.c \
../HAL/source/sys_main.c \
../HAL/source/sys_pcr.c \
../HAL/source/sys_phantom.c \
../HAL/source/sys_pmm.c \
../HAL/source/sys_selftest.c \
../HAL/source/sys_startup.c \
../HAL/source/sys_vim.c \
../HAL/source/system.c 

C_DEPS += \
./HAL/source/adc.d \
./HAL/source/crc.d \
./HAL/source/ecap.d \
./HAL/source/emac.d \
./HAL/source/eqep.d \
./HAL/source/errata_SSWF021_45.d \
./HAL/source/esm.d \
./HAL/source/etpwm.d \
./HAL/source/gio.d \
./HAL/source/i2c.d \
./HAL/source/lin.d \
./HAL/source/mdio.d \
./HAL/source/notification.d \
./HAL/source/os_croutine.d \
./HAL/source/os_event_groups.d \
./HAL/source/os_heap.d \
./HAL/source/os_list.d \
./HAL/source/os_mpu_wrappers.d \
./HAL/source/os_port.d \
./HAL/source/os_queue.d \
./HAL/source/os_tasks.d \
./HAL/source/os_timer.d \
./HAL/source/phy_dp83640.d \
./HAL/source/pinmux.d \
./HAL/source/rti.d \
./HAL/source/sci.d \
./HAL/source/spi.d \
./HAL/source/sys_dma.d \
./HAL/source/sys_main.d \
./HAL/source/sys_pcr.d \
./HAL/source/sys_phantom.d \
./HAL/source/sys_pmm.d \
./HAL/source/sys_selftest.d \
./HAL/source/sys_startup.d \
./HAL/source/sys_vim.d \
./HAL/source/system.d 

OBJS += \
./HAL/source/adc.obj \
./HAL/source/crc.obj \
./HAL/source/dabort.obj \
./HAL/source/ecap.obj \
./HAL/source/emac.obj \
./HAL/source/eqep.obj \
./HAL/source/errata_SSWF021_45.obj \
./HAL/source/esm.obj \
./HAL/source/etpwm.obj \
./HAL/source/gio.obj \
./HAL/source/i2c.obj \
./HAL/source/lin.obj \
./HAL/source/mdio.obj \
./HAL/source/notification.obj \
./HAL/source/os_croutine.obj \
./HAL/source/os_event_groups.obj \
./HAL/source/os_heap.obj \
./HAL/source/os_list.obj \
./HAL/source/os_mpu_wrappers.obj \
./HAL/source/os_port.obj \
./HAL/source/os_portasm.obj \
./HAL/source/os_queue.obj \
./HAL/source/os_tasks.obj \
./HAL/source/os_timer.obj \
./HAL/source/phy_dp83640.obj \
./HAL/source/pinmux.obj \
./HAL/source/rti.obj \
./HAL/source/sci.obj \
./HAL/source/spi.obj \
./HAL/source/sys_core.obj \
./HAL/source/sys_dma.obj \
./HAL/source/sys_intvecs.obj \
./HAL/source/sys_main.obj \
./HAL/source/sys_mpu.obj \
./HAL/source/sys_pcr.obj \
./HAL/source/sys_phantom.obj \
./HAL/source/sys_pmm.obj \
./HAL/source/sys_pmu.obj \
./HAL/source/sys_selftest.obj \
./HAL/source/sys_startup.obj \
./HAL/source/sys_vim.obj \
./HAL/source/system.obj 

ASM_DEPS += \
./HAL/source/dabort.d \
./HAL/source/os_portasm.d \
./HAL/source/sys_core.d \
./HAL/source/sys_intvecs.d \
./HAL/source/sys_mpu.d \
./HAL/source/sys_pmu.d 

OBJS__QUOTED += \
"HAL/source/adc.obj" \
"HAL/source/crc.obj" \
"HAL/source/dabort.obj" \
"HAL/source/ecap.obj" \
"HAL/source/emac.obj" \
"HAL/source/eqep.obj" \
"HAL/source/errata_SSWF021_45.obj" \
"HAL/source/esm.obj" \
"HAL/source/etpwm.obj" \
"HAL/source/gio.obj" \
"HAL/source/i2c.obj" \
"HAL/source/lin.obj" \
"HAL/source/mdio.obj" \
"HAL/source/notification.obj" \
"HAL/source/os_croutine.obj" \
"HAL/source/os_event_groups.obj" \
"HAL/source/os_heap.obj" \
"HAL/source/os_list.obj" \
"HAL/source/os_mpu_wrappers.obj" \
"HAL/source/os_port.obj" \
"HAL/source/os_portasm.obj" \
"HAL/source/os_queue.obj" \
"HAL/source/os_tasks.obj" \
"HAL/source/os_timer.obj" \
"HAL/source/phy_dp83640.obj" \
"HAL/source/pinmux.obj" \
"HAL/source/rti.obj" \
"HAL/source/sci.obj" \
"HAL/source/spi.obj" \
"HAL/source/sys_core.obj" \
"HAL/source/sys_dma.obj" \
"HAL/source/sys_intvecs.obj" \
"HAL/source/sys_main.obj" \
"HAL/source/sys_mpu.obj" \
"HAL/source/sys_pcr.obj" \
"HAL/source/sys_phantom.obj" \
"HAL/source/sys_pmm.obj" \
"HAL/source/sys_pmu.obj" \
"HAL/source/sys_selftest.obj" \
"HAL/source/sys_startup.obj" \
"HAL/source/sys_vim.obj" \
"HAL/source/system.obj" 

C_DEPS__QUOTED += \
"HAL/source/adc.d" \
"HAL/source/crc.d" \
"HAL/source/ecap.d" \
"HAL/source/emac.d" \
"HAL/source/eqep.d" \
"HAL/source/errata_SSWF021_45.d" \
"HAL/source/esm.d" \
"HAL/source/etpwm.d" \
"HAL/source/gio.d" \
"HAL/source/i2c.d" \
"HAL/source/lin.d" \
"HAL/source/mdio.d" \
"HAL/source/notification.d" \
"HAL/source/os_croutine.d" \
"HAL/source/os_event_groups.d" \
"HAL/source/os_heap.d" \
"HAL/source/os_list.d" \
"HAL/source/os_mpu_wrappers.d" \
"HAL/source/os_port.d" \
"HAL/source/os_queue.d" \
"HAL/source/os_tasks.d" \
"HAL/source/os_timer.d" \
"HAL/source/phy_dp83640.d" \
"HAL/source/pinmux.d" \
"HAL/source/rti.d" \
"HAL/source/sci.d" \
"HAL/source/spi.d" \
"HAL/source/sys_dma.d" \
"HAL/source/sys_main.d" \
"HAL/source/sys_pcr.d" \
"HAL/source/sys_phantom.d" \
"HAL/source/sys_pmm.d" \
"HAL/source/sys_selftest.d" \
"HAL/source/sys_startup.d" \
"HAL/source/sys_vim.d" \
"HAL/source/system.d" 

ASM_DEPS__QUOTED += \
"HAL/source/dabort.d" \
"HAL/source/os_portasm.d" \
"HAL/source/sys_core.d" \
"HAL/source/sys_intvecs.d" \
"HAL/source/sys_mpu.d" \
"HAL/source/sys_pmu.d" 

C_SRCS__QUOTED += \
"../HAL/source/adc.c" \
"../HAL/source/crc.c" \
"../HAL/source/ecap.c" \
"../HAL/source/emac.c" \
"../HAL/source/eqep.c" \
"../HAL/source/errata_SSWF021_45.c" \
"../HAL/source/esm.c" \
"../HAL/source/etpwm.c" \
"../HAL/source/gio.c" \
"../HAL/source/i2c.c" \
"../HAL/source/lin.c" \
"../HAL/source/mdio.c" \
"../HAL/source/notification.c" \
"../HAL/source/os_croutine.c" \
"../HAL/source/os_event_groups.c" \
"../HAL/source/os_heap.c" \
"../HAL/source/os_list.c" \
"../HAL/source/os_mpu_wrappers.c" \
"../HAL/source/os_port.c" \
"../HAL/source/os_queue.c" \
"../HAL/source/os_tasks.c" \
"../HAL/source/os_timer.c" \
"../HAL/source/phy_dp83640.c" \
"../HAL/source/pinmux.c" \
"../HAL/source/rti.c" \
"../HAL/source/sci.c" \
"../HAL/source/spi.c" \
"../HAL/source/sys_dma.c" \
"../HAL/source/sys_main.c" \
"../HAL/source/sys_pcr.c" \
"../HAL/source/sys_phantom.c" \
"../HAL/source/sys_pmm.c" \
"../HAL/source/sys_selftest.c" \
"../HAL/source/sys_startup.c" \
"../HAL/source/sys_vim.c" \
"../HAL/source/system.c" 

ASM_SRCS__QUOTED += \
"../HAL/source/dabort.asm" \
"../HAL/source/os_portasm.asm" \
"../HAL/source/sys_core.asm" \
"../HAL/source/sys_intvecs.asm" \
"../HAL/source/sys_mpu.asm" \
"../HAL/source/sys_pmu.asm" 


