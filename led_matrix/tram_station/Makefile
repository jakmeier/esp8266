CC = xtensa-lx106-elf-gcc
PW = $(shell cat password)
CFLAGS = -I. -mlongcalls -I ../../sdk/include -DPASSWORD=$(PW) 
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -T ../../sdk/ld/eagle.app.v6.ld -L ../../sdk/lib

main-0x00000.bin: main
	esptool.py elf2image $^

main: main.o led_matrix.o sbb.o jmr_http_helper.o jmr_date.o

main.o: main.c

sbb.o: sbb.c

led_matrix.o: led_matrix.c

jmr_http_helper.o: jmr_http_helper.c

jmr_date.o: jmr_date.c

flash: main-0x00000.bin
	esptool.py write_flash 0 main-0x00000.bin 0x10000 main-0x10000.bin

test-read:
	esptool.py --port /dev/ttyUSB0 read_flash 0x00000 0xFFFF backup-0x00000.bin

setpath:
	@echo "Use source ../../sourcePathFile" #	export PATH=/../../xtensa-lx106-elf/bin:$$PATH

clean:
	rm -f main main.o led_matrix.o jmr_http_helper.o jmr_date.o sbb.o main-0x00000.bin main-0x10000.bin
