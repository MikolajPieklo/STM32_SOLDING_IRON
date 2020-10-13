#Author: M Pieklo
#Date: 03.10.2020
#Project: Soldering iron based on STM32.
#License: Opensource

CC=arm-none-eabi-gcc
CC2HEX=arm-none-eabi-objcopy
MACH=cortex-m3

CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -O0 -DSTM32F103xB -specs=nano.specs -Wall
#-MF"Core/Src/system_stm32f1xx.d" -MT"Core/Src/system_stm32f1xx.o" 
DEBUGINFO= -DDEBUG -g3 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -mfloat-abi=soft 
CONST= -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSI_VALUE=8000000 -DLSE_VALUE=32768 -DLSI_VALUE=40000 
#'-DHSE_STARTUP_TIMEOUT=100' '-DLSE_STARTUP_TIMEOUT=5000''-DVDD_VALUE=3300' '-DPREFETCH_ENABLE=1' 

INCLUDES= -I Core/Inc/ -I Drivers/STM32F1xx_HAL_Driver/Inc/ -I Drivers/CMSIS/Device/ST/STM32F1xx/Include/ -I Drivers/CMSIS/Include/

LDFLAGS= --specs=nosys.specs -Wl,-Map="build/test.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb\
 -Wl,--start-group -lc -lm -Wl,--end-group -T STM32F103C8TX_FLASH.ld
#-nostdlib

all: build/target.elf build/target.hex
 
build/main.o: Core/Src/main.c Core/Inc/main.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/main.c -o build/main.o
	#                           		 			  $^              -o $@
	
build/HD44780.o: Core/Src/HD44780.c Core/Inc/HD44780.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/HD44780.c -o build/HD44780.o

build/adc.o: Core/Src/adc.c Core/Inc/adc.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/adc.c -o build/adc.o	
	
build/gpio.o: Core/Src/gpio.c Core/Inc/gpio.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/gpio.c -o build/gpio.o	
	
build/tim.o: Core/Src/tim.c Core/Inc/tim.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/tim.c -o build/tim.o	
	
build/myDelay.o: Core/Src/myDelay.c Core/Inc/myDelay.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/myDelay.c -o build/myDelay.o	
	
build/sys.o: Core/Src/sys.c Core/Inc/sys.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/sys.c -o build/sys.o	
	
build/syscalls.o: Core/Src/syscalls.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/syscalls.c -o build/syscalls.o	
	
build/sysmem.o: Core/Src/sysmem.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/sysmem.c -o build/sysmem.o	
	
build/stm32f1xx_it.o: Core/Src/stm32f1xx_it.c Core/Inc/stm32f1xx_it.h
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/stm32f1xx_it.c -o build/stm32f1xx_it.o	
	
build/system_stm32f1xx.o: Core/Src/system_stm32f1xx.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) Core/Src/system_stm32f1xx.c -o build/system_stm32f1xx.o
	
build/startup_stm32f103c8tx.o:	Core/Startup/startup_stm32f103c8tx.s
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) -o $@ $^
	
build/stm32f1xx_ll_adc.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_dma.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_exti.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_gpio.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_pwr.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_rcc.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_tim.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) $(INCLUDES) -o $@ $^
	
build/stm32f1xx_ll_utils.o: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c
	$(CC) $(CFLAGS) $(CONST) $(DEBUGINFO) ${INCLUDES} -o $@ $^
		
build/target.elf: build/main.o  build/stm32f1xx_ll_adc.o build/stm32f1xx_ll_dma.o\
build/stm32f1xx_ll_exti.o	build/stm32f1xx_ll_gpio.o build/stm32f1xx_ll_pwr.o build/stm32f1xx_ll_rcc.o\
build/stm32f1xx_ll_tim.o build/stm32f1xx_ll_utils.o build/system_stm32f1xx.o build/HD44780.o build/gpio.o build/tim.o\
build/adc.o build/myDelay.o build/sys.o build/stm32f1xx_it.o build/syscalls.o build/sysmem.o build/startup_stm32f103c8tx.o
	$(CC) $(LDFLAGS) $^ -o $@

build/target.hex: Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c
	$(CC2HEX) -O ihex build/target.elf build/target.hex
	arm-none-eabi-size build/target.elf
	arm-none-eabi-objdump -h -S  build/target.elf > build/target.list
	arm-none-eabi-objcopy -O binary  build/target.elf  build/target.bin
	 
clean:
	rm -rf build/*.o build/*.d build/*.elf build/*.hex build/*.list build/*.bin build/*.map build/*.su
	
load:
	openocd -f board/stm32f4discovery.cfg
