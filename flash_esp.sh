import os

# env varibles
os.environ['MICROPYTHON'] = os.getcwd()
os.environ['PATH'] = os.environ['PATH'] + ':' + os.environ['MICROPYTHON']+'/xtensa-esp32-elf/bin'
os.environ['ESPIDF'] = os.environ['MICROPYTHON'] + 'esp-idf'

os.system('esptool.py --port /dev/tty.usbserial-A50285BI --baud 115200 erase_flash')
os.chdir(os.environ['MICROPYTHON'] + '/micropython/ports/esp32')
os.system('make deploy')