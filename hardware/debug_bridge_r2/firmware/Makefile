SOURCES := main.c hardware.c vbus.c

OBJECTS := $(patsubst %.c,%.o,${SOURCES})

CC := avr-gcc
OBJDUMP := avr-objdump
SIZE := avr-size
AVRDUDE := avrdude

GCC_CHIP=attiny24a
AVRDUDE_CHIP=t24

AVRDUDE_PROGRAMMER=atmelice_isp

CFLAGS := -O2 -flto -g -Wall -Wextra -mmcu=${GCC_CHIP}
LDFLAGS :=

.PHONY: all clean program fuses

all: disasm.txt

fw.elf: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} $^ -o $@
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	${SIZE} $@
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

disasm.txt: fw.elf
	${OBJDUMP} -S $< > $@

# Fuses:
# Lfuse = 0xe2: 8 MHz, fast start, no clock div/8
# Hfuse = 0xdb: BOD at 2.3V, SPI programming enabled
# Efuse = 0xff: self-programming disabled
fuses:
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U lfuse:w:0xc2:m -U hfuse:w:0xdb:m -U efuse:w:0xff:m

program: fw.elf
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U flash:w:fw.elf

clean:
	rm -f ${OBJECTS}
