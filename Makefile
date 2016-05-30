SOURCES=main.c display.c stack_init.S
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=preempt-sched

CC=avr-gcc
CFLAGS=-Wall -Os -Werror -Wextra
PFLAGS?=
AVRTYPE=atmega168a

all: $(SOURCES) $(EXECUTABLE) hex

clean:
	rm -f *.o *.elf *.lst *.hex *.decompiled

.c.o:
	$(CC) $(CFLAGS) $(PFLAGS) -mmcu=$(AVRTYPE) -Wa,-ahlmns=$(EXECUTABLE).lst -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -mmcu=$(AVRTYPE) $(OBJECTS) -o $@.elf
	chmod a-x $(EXECUTABLE).elf 2>&1

hex: $(EXECUTABLE)
	avr-objcopy -j .text -j .data -O ihex $(EXECUTABLE).elf $(EXECUTABLE).flash.hex
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $(EXECUTABLE).elf $(EXECUTABLE).eeprom.hex

flash: hex
	sudo avrdude -p m168 -c usbtiny -v -U flash:w:$(EXECUTABLE).flash.hex

decompile: hex
	avr-objdump -s -m avr:5 -D $(EXECUTABLE).flash.hex > $(EXECUTABLE).decompiled
