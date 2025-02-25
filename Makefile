##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.18.0-B7] date: [Mon Aug 05 16:47:50 CST 2024]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = demo_gd32

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/main.c \
Core/Src/systick.c \
Core/Src/gd32e23x_it.c \
Core/Src/log.c \
Core/Src/app.c \
Core/Src/peripheral.c \
Drivers/GD32E23x/Source/gd32e23x_adc.c \
Drivers/GD32E23x/Source/gd32e23x_cmp.c \
Drivers/GD32E23x/Source/gd32e23x_crc.c \
Drivers/GD32E23x/Source/gd32e23x_dbg.c \
Drivers/GD32E23x/Source/gd32e23x_dma.c \
Drivers/GD32E23x/Source/gd32e23x_exti.c \
Drivers/GD32E23x/Source/gd32e23x_fmc.c \
Drivers/GD32E23x/Source/gd32e23x_fwdgt.c \
Drivers/GD32E23x/Source/gd32e23x_gpio.c \
Drivers/GD32E23x/Source/gd32e23x_i2c.c \
Drivers/GD32E23x/Source/gd32e23x_misc.c \
Drivers/GD32E23x/Source/gd32e23x_pmu.c \
Drivers/GD32E23x/Source/gd32e23x_rcu.c \
Drivers/GD32E23x/Source/gd32e23x_rtc.c \
Drivers/GD32E23x/Source/gd32e23x_spi.c \
Drivers/GD32E23x/Source/gd32e23x_syscfg.c \
Drivers/GD32E23x/Source/gd32e23x_timer.c \
Drivers/GD32E23x/Source/gd32e23x_usart.c \
Drivers/GD32E23x/Source/gd32e23x_wwdgt.c \
Drivers/CMSIS/GD/GD32E23x/Source/syscalls.c \
Drivers/CMSIS/GD/GD32E23x/Source/system_gd32e23x.c \
Drivers/UserDrivers/lcd.c
# Core/Src/gd32e230c_eval.c

# ASM sources
ASM_SOURCES =  \
startup_gd32e23x.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m23

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/GD32E23x/Include \
-IDrivers/CMSIS \
-IDrivers/CMSIS/GD32E23x/Include \
-IDrivers/CMSIS/GD/GD32E23x/Include \
-IDrivers/UserDrivers

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -std=c99 -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_INCLUDES) $(OPT) -std=c99 -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
# CFLAGS += -fexec-charset=gbk

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = gd32e23x_flash.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAGS += -u_printf_float -u_sprintf_float -u_vsprintf_float -u_scanf_float -u_sscanf_float -u_vsscanf_float

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-del /q $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

#######################################
# openocd
#######################################
INTERFACE_CFG=openocd_gdlink.cfg

# Replace Your File
TARGET_CFG=xxxxxxxxxxx.cfg

download:
	-openocd -f ${INTERFACE_CFG} -f ${TARGET_CFG} -c init -c "reset halt; wait_halt; flash write_image erase build/${TARGET}.bin 0x8000000" -c reset -c shutdown

debug:
	-openocd -f ${INTERFACE_CFG} -f ${TARGET_CFG}

# *** EOF ***