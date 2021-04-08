# Author: M Pieklo
# Date: 14.12.2020
# Project: Soldering iron based on STM32.
# License: Opensource

ccblack = \033[0;30m
ccred = \033[0;31m
ccgreen = \033[0;32m
ccyellow = \033[0;33m
ccblue = \033[0;34m
ccpurple = \033[0;35m
cccyan = \033[0;36m
ccend = \033[0m

$(info --------------------------)
USER := $(shell whoami)
$(info User: $(USER))

DATE := $(shell date +"%d.%m.%Y")
$(info Date: $(DATE))

TIME := $(shell date +"%H:%M:%S")
$(info Time: $(TIME))

GCC_PATH := $(shell which arm-none-eabi-gcc)
$(info GCC_PATH: $(GCC_PATH))

ifneq ($(shell test -e $(GCC_PATH) && echo -n yes),yes)
$(error error: ARM package '$(GCC_PATH)' does not exist!)
else
CC_VERSION := $(shell arm-none-eabi-gcc -dumpversion)
CC := arm-none-eabi-gcc
CC2HEX := arm-none-eabi-objcopy
$(info Compiler version: $(CC_VERSION))
endif
$(info --------------------------)

VERSION := $(shell git describe --tags)
$(info Version: $(VERSION))
COMMIT_ID := $(shell git rev-parse HEAD)
$(info Commit ID: $(COMMIT_ID))
$(info --------------------------)

BUILD_DIR := build
DRIVER_DIR := $(BUILD_DIR)/Drivers
NAME := $(BUILD_DIR)/TARGET

MACH=cortex-m3

# Use newlib.
USE_NOHOST=--specs=nosys.specs
USE_NANO=--specs=nano.specs
USE_SEMIHOST=--specs=rdimon.specs

# Create map file
MAP=-Wl,-Map=$(NAME).map

# Link for code size
GC=-Wl,--gc-sections

CFLAGS= -c -mcpu=$(MACH) -mthumb -mfloat-abi=soft -std=gnu11 -O0 -DSTM32F103xB $(USE_NANO) -Wall \
	-ffunction-sections -fdata-sections -fstack-usage -MMD 
	
DEBUGINFO= -DDEBUG -g3 

CONST= -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSI_VALUE=8000000 -DLSE_VALUE=32768 -DLSI_VALUE=40000 

INCLUDES= -ICore/Inc/ -IDrivers/STM32F1xx_HAL_Driver/Inc/ -IDrivers/CMSIS/Device/ST/STM32F1xx/Include/ -IDrivers/CMSIS/Include/

LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft -T"STM32F103C8TX_FLASH.ld" \
	$(MAP) $(GC) -static $(USE_NANO)  -Wl,--start-group -lc -lm -Wl,--end-group 

all: make$(BUILD_DIR) $(BUILD_DIR)/target.elf $(BUILD_DIR)/target.hex
	
make$(BUILD_DIR):	
	@if [ ! -e $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	@if [ ! -e $(DRIVER_DIR) ]; then mkdir $(DRIVER_DIR); fi

$(BUILD_DIR)/main.o: Core/Src/main.c Core/Inc/main.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/main.c -o $(BUILD_DIR)/main.o
	#                           		 			  $^              -o $@
	
$(BUILD_DIR)/HD44780.o: Core/Src/HD44780.c Core/Inc/HD44780.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/HD44780.c -o $(BUILD_DIR)/HD44780.o

$(BUILD_DIR)/adc.o: Core/Src/adc.c Core/Inc/adc.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/adc.c -o $(BUILD_DIR)/adc.o	
	
$(BUILD_DIR)/gpio.o: Core/Src/gpio.c Core/Inc/gpio.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/gpio.c -o $(BUILD_DIR)/gpio.o	
	
$(BUILD_DIR)/tim.o: Core/Src/tim.c Core/Inc/tim.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/tim.c -o $(BUILD_DIR)/tim.o	
	
$(BUILD_DIR)/myDelay.o: Core/Src/myDelay.c Core/Inc/myDelay.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/myDelay.c -o $(BUILD_DIR)/myDelay.o	
	
$(BUILD_DIR)/sys.o: Core/Src/sys.c Core/Inc/sys.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/sys.c -o $(BUILD_DIR)/sys.o	
	
$(BUILD_DIR)/syscalls.o: Core/Src/syscalls.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/syscalls.c -o $(BUILD_DIR)/syscalls.o	
	
$(BUILD_DIR)/sysmem.o: Core/Src/sysmem.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/sysmem.c -o $(BUILD_DIR)/sysmem.o	
	
$(BUILD_DIR)/stm32f1xx_it.o: Core/Src/stm32f1xx_it.c Core/Inc/stm32f1xx_it.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/stm32f1xx_it.c -o $(BUILD_DIR)/stm32f1xx_it.o	
	
$(BUILD_DIR)/system_stm32f1xx.o: Core/Src/system_stm32f1xx.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/system_stm32f1xx.c -o $(BUILD_DIR)/system_stm32f1xx.o
	
$(BUILD_DIR)/startup_stm32f103c8tx.o:	Core/Startup/startup_stm32f103c8tx.s
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_adc.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_dma.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_exti.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_gpio.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_pwr.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_rcc.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_tim.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
$(DRIVER_DIR)/stm32f1xx_ll_utils.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) ${INCLUDES} -o $@ $^

$(BUILD_DIR)/target.elf: \
	$(BUILD_DIR)/main.o \
	$(DRIVER_DIR)/stm32f1xx_ll_adc.o \
	$(DRIVER_DIR)/stm32f1xx_ll_dma.o \
	$(DRIVER_DIR)/stm32f1xx_ll_exti.o \
	$(DRIVER_DIR)/stm32f1xx_ll_gpio.o \
	$(DRIVER_DIR)/stm32f1xx_ll_pwr.o \
	$(DRIVER_DIR)/stm32f1xx_ll_rcc.o\
	$(DRIVER_DIR)/stm32f1xx_ll_tim.o \
	$(DRIVER_DIR)/stm32f1xx_ll_utils.o \
	$(BUILD_DIR)/system_stm32f1xx.o \
	$(BUILD_DIR)/HD44780.o \
	$(BUILD_DIR)/gpio.o \
	$(BUILD_DIR)/tim.o \
	$(BUILD_DIR)/adc.o \
	$(BUILD_DIR)/myDelay.o \
	$(BUILD_DIR)/sys.o \
	$(BUILD_DIR)/stm32f1xx_it.o \
	$(BUILD_DIR)/syscalls.o \
	$(BUILD_DIR)/sysmem.o \
	$(BUILD_DIR)/startup_stm32f103c8tx.o
		@echo "$(ccblue)\nLinking$(ccend)"
		$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/target.hex: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c
	$(CC2HEX) -O ihex $(BUILD_DIR)/target.elf $(BUILD_DIR)/target.hex
		
	@echo "$(ccblue)\nGenerating list file$(ccend)"
	arm-none-eabi-objdump -h -S  $(BUILD_DIR)/target.elf > $(BUILD_DIR)/target.list
	
	@echo "$(ccblue)\nCreating binary file$(ccend)"
	arm-none-eabi-objcopy -O binary  $(BUILD_DIR)/target.elf  $(BUILD_DIR)/target.bin
	
	@echo "$(ccpurple)"
	arm-none-eabi-size build/target.elf -A -x
	@echo "$(ccend)"
	 
clean:
	rm -rf $(BUILD_DIR)
	
load:
	openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/local/share/openocd/scripts/target/stm32f1x.cfg
	
	
	.PHONY all make$(BUILD_DIR) clean load
