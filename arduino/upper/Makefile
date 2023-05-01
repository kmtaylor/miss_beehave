# Arduino.h requires that F_CPU be greater than 1000000
F_CPU    := 16000000UL
CPU_TYPE := 2560

GCC_VER_GE12 := $(shell [ `avr-gcc -dumpversion | \
                           sed 's/\..*//'` -ge 12 ] && echo true)

CPPFLAGS = -Wall -Os -mmcu=atmega$(CPU_TYPE) -DF_CPU=$(F_CPU)
CPPFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS += -ffunction-sections -fdata-sections
CPPFLAGS += -DARDUINO=200
CPPFLAGS += $(if $(GCC_VER_GE12), --param=min-pagesize=0)

ARDUINO_INCLUDE := -I ../lib/arduino_lib -I ../lib/modbusino -I ../common
ARDUINO_INCLUDE += -I ../lib/iBus/src
INCLUDE := $(ARDUINO_INCLUDE)

LDFLAGS := -B /usr/avr/lib/avr5 -Wl,--gc-sections

ARDUINO_OBJS := ../lib/arduino_lib/wiring.o \
                ../lib/arduino_lib/wiring_digital.o \
                ../lib/arduino_lib/hooks.o \
                ../lib/arduino_lib/HardwareSerial.o \
                ../lib/arduino_lib/HardwareSerial0.o \
                ../lib/arduino_lib/HardwareSerial2.o \
                ../lib/arduino_lib/Print.o \
                ../lib/arduino_lib/main.o \
                ../lib/modbusino/Modbusino.o \
		../lib/iBus/src/iBus.o

OBJS = upper.o

FORMAT = ihex

TARGET = upper

all: $(TARGET).hex $(TARGET).eep $(TARGET).lss

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O $(FORMAT) -R .eeprom $< $@
$(TARGET).eep: $(TARGET).elf
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
		--change-section-lma .eeprom=0 --no-change-warnings  \
		-O $(FORMAT) $< $@

$(TARGET).lss: $(TARGET).elf
	avr-objdump -h -S $< > $@

$(TARGET).elf: $(OBJS) $(ARDUINO_OBJS)
	avr-g++ $(CPPFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@
	avr-size $@

%.o: %.cpp
	avr-g++ $(CPPFLAGS) $(INCLUDE) -c $< -o $@
%.o: %.c
	avr-gcc $(CPPFLAGS) $(INCLUDE) -c $< -o $@

program: $(TARGET).hex
	avrdude -p m$(CPU_TYPE) -c wiring -D -U flash:w:$(TARGET).hex \
		-P /dev/ttyACM0
clean:
	rm -f *.o
	rm -f ../common/*.o
	rm -f ../lib/arduino_lib/*.o
	rm -f ../lib/modbusino/*.o
	rm -f ../lib/iBus/src/*.o

cleanall: clean
	rm -f $(TARGET).hex
	rm -f $(TARGET).lss
	rm -f $(TARGET).eep
	rm -f $(TARGET).elf
