SOURCES := main.c hardware.c

OBJECTS := $(patsubst %.c,%.o,${SOURCES})

CC := avr-gcc
OBJDUMP := avr-objdump
SIZE := avr-size

CFLAGS := -O2 -flto -g -Wall -Wextra -mmcu=attiny24a
LDFLAGS :=

.PHONY: all clean

all: disasm.txt

fw.elf: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} $^ -o $@
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	${SIZE} $@
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

disasm.txt: fw.elf
	${OBJDUMP} -S $< > $@

clean:
	rm -f ${OBJECTS}
