import os, subprocess


# Set Env
os.system('virtualenv -p python3 venv/bin/activate')

# env varibles
os.environ['MICROPYTHON'] = os.getcwd()
os.environ['PATH'] = os.environ['PATH'] + ':' + os.environ['MICROPYTHON']+'/xtensa-esp32-elf/bin'
os.environ['ESPIDF'] = os.environ['MICROPYTHON'] + 'esp-idf'

# Download firmware
os.system('git clone https://github.com/edo92/micropython-firmware.git')
os.rename('micropython-firmware', 'firmware')

# Setup esp-idf
os.environ['MICROPYTHON'] = os.getcwd()
os.system('git clone https://github.com/espressif/esp-idf.git')
os.chdir('esp-idf')
os.system('git checkout 5c88c5996dbde6208e3bec05abc21ff6cd822d26')
os.system('git submodule update --init --recursive')

# setup Micropython
os.chdir(os.environ['MICROPYTHON'])
os.system('git clone https://github.com/tsaarni/micropython-with-esp32-cam.git')
os.rename('micropython-with-esp32-cam', 'micropython')
os.chdir('micropython')
os.system('git submodule update --init')


# Setup camera for Micropython
os.chdir(os.environ['MICROPYTHON'] + '/esp-idf/components')
os.system('git clone https://github.com/tsaarni/esp32-camera-for-micropython.git')
os.rename('esp32-camera-for-micropython', 'esp-camera')
os.environ['ESPIDF'] = os.environ['MICROPYTHON'] + '/esp-idf'


# Change make file
print('============================')
print('Change data in makefile')
print('line 17 change PORT')
print('line 22 change CROSS_COMPILE to -> "firmware-" ')
print('============================')

subprocess.call(('open', 'Makefile'))
