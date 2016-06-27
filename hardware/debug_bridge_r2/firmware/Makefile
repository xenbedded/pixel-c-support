SOURCES := main.c hardware.c vbus.c

OBJECTS := $(patsubst %.c,%.o,${SOURCES})

CC := avr-gcc
OBJDUMP := avr-objdump
SIZE := avr-size
AVRDUDE := avrdude

GCC_CHIP=attiny48
AVRDUDE_CHIP=t48
F_CPU=8000000

AVRDUDE_PROGRAMMER=atmelice_isp

CFLAGS := -O2 -flto -g -Wall -Wextra -mmcu=${GCC_CHIP} -DF_CPU=${F_CPU}uL
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
# Lfuse = 0x8e: 8 MHz, fast start, no clock div/8, clock out on PB0
# Hfuse = 0xdd: BOD at 2.7V, SPI programming enabled
# Efuse = 0xff: self-programming disabled
LFUSE := 0x8e
HFUSE := 0xdd
EFUSE := 0xff
fuses:
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U lfuse:w:${LFUSE}:m -U hfuse:w:${HFUSE}:m -U efuse:w:${EFUSE}:m

program: fw.elf
	avrdude -p ${AVRDUDE_CHIP} -c ${AVRDUDE_PROGRAMMER} -U flash:w:fw.elf

clean:
	rm -f ${OBJECTS} fw.elf disasm.txt
