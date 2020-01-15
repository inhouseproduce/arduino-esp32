PORT=/dev/tty.SLAB_USBtoUART
BINFILE=esp8266-20191220-v1.12.bin

default: put run

put:
	ampy --port ${PORT} put main.py
	ampy --port ${PORT} put boot.py
	ampy --port ${PORT} put sht31.py

run:
	ampy --port ${PORT} run main.py

clean:
	ampy --port ${PORT} rm main.py
	ampy --port ${PORT} rm boot.py
	ampy --port ${PORT} rm sht31.py

install:
	chmod +x run.sh
	./run.sh

flash: erase_fl write_fl

erase_fl:
	esptool.py --port ${PORT} erase_flash

write_fl:
	esptool.py --port ${PORT} --baud 115200 write_flash --flash_size=detect 0 ${BINFILE}
