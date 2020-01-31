import os

# env varibles
os.environ['MICROPYTHON'] = os.getcwd()
os.environ['PATH'] = os.environ['PATH'] + ':' + os.environ['MICROPYTHON']+'/xtensa-esp32-elf/bin'
os.environ['ESPIDF'] = os.environ['MICROPYTHON'] + 'esp-idf'

os.environ['PATH'] = os.environ['PATH'] + ':' + os.environ['MICROPYTHON']+'/xtensa-esp32-elf/bin'
os.environ['ESPIDF'] = os.environ['MICROPYTHON'] + 'esp-idf'
os.chdir(os.environ['MICROPYTHON'] + '/micropython/ports/esp32')
os.system('make V=1 SDKCONFIG=boards/sdkconfig.esp32cam -j')