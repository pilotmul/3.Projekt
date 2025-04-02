SOURCES = main.c uart.c ftoa.c PWM.c SCD30.c PIDControl.c
OBJECTS = $(SOURCES:.c=.o)
ELF_FILE = prog.elf
HEX_FILE = prog.hex
TARGETS = upload build clean help
MMCU = atmega2560
PORT = /dev/cu.usbmodem*
PROGRAMMER = wiring
UPLOAD_BAUDRATE = 115200
CC = avr-gcc
CCFLAGS = -Os -DF_CPU=16000000UL -mmcu=$(MMCU) -I.

.PHONY : clean help build upload serial

#compiles program and uploads to mega2560
all: $(HEX_FILE)
	avrdude -v -V -c $(PROGRAMMER) -p $(MMCU) -P $(PORT) -b $(UPLOAD_BAUDRATE) -D -U flash:w:$(HEX_FILE):i

#	// Original code for compiling and uploading to mega2560 \\ #
#	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega2560 -o main.elf main.c
# 	avr-gcc -o main.bin main.elf
#	avr-objcopy -O ihex -R .eeprom main.elf main.hex
#	sudo avrdude -v -V -c wiring -p atmega2560 -P /dev/cu.usbmodem101 -b 115200 -D -U flash:w:main.hex:i

#upload .hex file to mega2560
upload : $(HEX_FILE)
	avrdude -v -V -c $(PROGRAMMER) -p $(MMCU) -P $(PORT) -b $(UPLOAD_BAUDRATE) -D -U flash:w:$(HEX_FILE):i

#build all files
build : $(HEX_FILE)

#convert .elf to .hex file
$(HEX_FILE) : $(ELF_FILE)
	avr-objcopy -O ihex -R .eeprom $(ELF_FILE) $(HEX_FILE)

#create .elf file from output files
$(ELF_FILE) : $(OBJECTS)
	$(CC) $(CCFLAGS) -o $(ELF_FILE) $(OBJECTS)

#compile output files from .c source files
%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean : 
	rm -f $(OBJECTS) $(HEX_FILE) $(ELF_FILE)

help : 
	echo $(TARGETS)

#opens to serial port using screen CLI. press "ctrl + A", then "D" to exit
serial :
	stty -f /dev/cu.usbmodem101 9600 & screen /dev/cu.usbmodem101 9600
#Quits all screen sessions. reason being when detaching a screen session using ctrl + A, followed by  D. The session is exited but kept running
	screen -X quit