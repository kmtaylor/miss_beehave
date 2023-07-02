TARGET := hello_world

TOOLCHAIN := ../lib/xpack-arm-none-eabi-gcc-12.2.1-1.2/bin

ARDUINO_CORE := ../lib/Arduino_Core_STM32
ARDUINO_MIDDLEWARES := $(ARDUINO_CORE)/system/Middlewares
ARDUINO_DRIVERS     := $(ARDUINO_CORE)/system/Drivers

VARIANT := STM32F2xx/F207V(C-E-F-G)T_F217V(E-G)T

# Needed for dealing with mal-formed VARIANT name
quote = $(patsubst %, "%", $(1))

ARM-G++ := $(TOOLCHAIN)/arm-none-eabi-g++
ARM-GCC := $(TOOLCHAIN)/arm-none-eabi-gcc
ARM-OBJCOPY := $(TOOLCHAIN)/arm-none-eabi-objcopy

all: hello_world.dfu

program:
	dfu-util -D $(TARGET).dfu

OBJS := hello_world.o
OBJS += ../lib/modbusino/Modbusino.o

CFLAGS := -DARDUINO_ARCH_STM32
CFLAGS += -DARDUINO_GENERIC_F207VCTX
CFLAGS += -DBOARD_ID=GENERIC_F207VCTX 
CFLAGS += -DBOARD_NAME=\"GENERIC_F207VCTX\"
CFLAGS += -DVARIANT_H=\"variant_generic.h\" 
CFLAGS += -DNDEBUG -DSTM32F2xx -DSTM32F207xx
CFLAGS += -DUSE_FULL_LL_DRIVER 
CFLAGS += -DUSBCON -DUSBD_VID=0 -DUSBD_PID=0
CFLAGS += -DHAL_UART_MODULE_ENABLED
CFLAGS += -DHAL_PCD_MODULE_ENABLED -DUSBD_USE_CDC
CFLAGS += -DARDUINO=200
CFLAGS += --param max-inline-insns-single=500 
CFLAGS += -ffunction-sections -fdata-sections -fno-exceptions 
CFLAGS += -Os -mcpu=cortex-m3 -mthumb

C++FLAGS := -std=gnu++17 -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics

LDFLAGS := -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler 
LDFLAGS += -Wl,--unresolved-symbols=report-all -Wl,--warn-common 
LDFLAGS += -Wl,--script=../lib/Arduino_Core_STM32/system/ldscript.ld
LDFLAGS += -Wl,--default-script=$\
               "$(ARDUINO_CORE)/variants/$(VARIANT)/ldscript.ld" 
LDFLAGS += -Wl,--defsym=LD_FLASH_OFFSET=0x0 
LDFLAGS += -Wl,--defsym=LD_MAX_SIZE=262144 
LDFLAGS += -Wl,--defsym=LD_MAX_DATA_SIZE=131072
LDFLAGS += --specs=nano.specs

INCLUDE := -I$(ARDUINO_CORE)/cores/arduino 
INCLUDE += -I$(ARDUINO_CORE)/cores/arduino/stm32 
INCLUDE += -I$(ARDUINO_CORE)/cores/arduino/stm32/LL 
INCLUDE += -I$(ARDUINO_CORE)/cores/arduino/stm32/usb 
INCLUDE += -I$(ARDUINO_CORE)/cores/arduino/stm32/usb/cdc 
INCLUDE += -I$(ARDUINO_MIDDLEWARES)/ST/STM32_USB_Device_Library/Core/Inc 
INCLUDE += -I$(ARDUINO_MIDDLEWARES)/ST/STM32_USB_Device_Library/Core/Src 
INCLUDE += -I../lib/CMSIS/CMSIS/Core/Include 
INCLUDE += -I$(ARDUINO_CORE)/system/STM32F2xx 
INCLUDE += -I$(ARDUINO_CORE)/system/Drivers/STM32F2xx_HAL_Driver/Inc 
INCLUDE += -I$(ARDUINO_CORE)/system/Drivers/STM32F2xx_HAL_Driver/Src 
INCLUDE += -I$(ARDUINO_DRIVERS)/CMSIS/Device/ST/STM32F2xx/Include 
INCLUDE += -I$(ARDUINO_DRIVERS)/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc
INCLUDE += -I$(ARDUINO_CORE)/variants/$(VARIANT)
INCLUDE += -I ../lib/modbusino 
INCLUDE := $(call quote, $(INCLUDE))

OBJS += $(ARDUINO_CORE)/cores/arduino/main.o
OBJS += $(ARDUINO_CORE)/cores/arduino/hooks.o
OBJS += $(ARDUINO_CORE)/cores/arduino/board.o
OBJS += $(ARDUINO_CORE)/cores/arduino/Print.o
OBJS += $(ARDUINO_CORE)/cores/arduino/Stream.o
OBJS += $(ARDUINO_CORE)/cores/arduino/USBSerial.o
OBJS += $(ARDUINO_CORE)/cores/arduino/WSerial.o
OBJS += $(ARDUINO_CORE)/cores/arduino/HardwareSerial.o
OBJS += $(ARDUINO_CORE)/cores/arduino/wiring_time.o
OBJS += $(ARDUINO_CORE)/cores/arduino/wiring_digital.o
OBJS += $(ARDUINO_CORE)/cores/arduino/wiring_analog.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/startup_stm32yyxx.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/cdc/usbd_cdc_if.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/cdc/usbd_cdc.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/cdc/cdc_queue.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usbd_conf.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usbd_if.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usb_device_core.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usb_device_ioreq.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usb_device_ctlreq.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usbd_ep_conf.o
OBJS += $(ARDUINO_CORE)/cores/arduino/stm32/usb/usbd_desc.o
OBJS += $(ARDUINO_CORE)/variants/$(VARIANT)/generic_clock.o
OBJS += $(ARDUINO_CORE)/variants/$(VARIANT)/PeripheralPins.o
OBJS += $(ARDUINO_CORE)/variants/$(VARIANT)/variant_generic.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/syscalls.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HardwareTimer.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/system_stm32yyxx.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/clock.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/pinmap.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/uart.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/dwt.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/PortNames.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/hw_config.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/analog.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/stm32/timer.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_uart.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_dma.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_crc.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_pcd.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_pcd_ex.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_rcc.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/HAL/stm32yyxx_hal_cortex.o
OBJS += $(ARDUINO_CORE)/libraries/SrcWrapper/src/LL/stm32yyxx_ll_usb.o
OBJS += $(ARDUINO_DRIVERS)/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_dac.o
OBJS += $(ARDUINO_DRIVERS)/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_tim.o
OBJS += $(ARDUINO_DRIVERS)/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_tim_ex.o

$(TARGET).bin: $(TARGET).elf
	$(ARM-OBJCOPY) -O binary $< $@

$(TARGET).dfu: $(TARGET).bin
	../lib/dfuse-pack.py -b 0x08000000:$< $@

$(TARGET).elf: $(OBJS)
	$(ARM-G++) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(call quote, $^) -o $@
	# -larm_cortexM3l_math -lm -lstdc++ -lc -lgcc 
	#arm-none-eabi-size $@

%.o: %.cpp
	$(ARM-G++) $(CFLAGS) $(C++FLAGS) $(INCLUDE) -c "$<" -o "$@"

%.o: %.c
	$(ARM-GCC) $(CFLAGS) $(INCLUDE) -c "$<" -o "$@"

%.o: %.S
	$(ARM-GCC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(call quote, $(OBJS))

.PHONY: clean program
