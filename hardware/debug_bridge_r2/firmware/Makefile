SOURCES := main.c hardware.c vbus.c

OBJECTS := $(patsubst %.c,%.o,${SOURCES})

CC := avr-gcc
OBJDUMP := avr-objdump
SIZE := avr-size
AVRDUDE := avrdude

GCC_CHIP=attiny48
AVRDUDE_CHIP=t48

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
# Lfuse = 0xce: 8 MHz, fast start, no clock div/8
# Hfuse = 0xdd: BOD at 2.7V, SPI programming enabled
# Efuse = 0xff: self-programming disabled
fuses:
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U lfuse:w:0xce:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m

program: fw.elf
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U flash:w:fw.elf

clean:
	rm -f ${OBJECTS} fw.elf disasm.txt
