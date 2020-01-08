## From Scratch

## Tools
    pip install pyserial
    pip install pyparsing
    pip install esptool
    pip install virtualenv
    pip install adafruit-ampy
    brew install cmake ninja
    brew install ccache
    
## Tool chain
    brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake make

# Erase flash
    esptool.py --port /dev/tty.usbserial-A50285BI --baud 115200 erase_flash

# Micropython environment
    virtualenv -p python3 venv
    . venv/bin/activate

# firmware - download xtensa-esp32-elf file
    https://docs.espressif.com/projects/esp-idf/en/release-v3.0/get-started/macos-setup.html

# esp-idf
    export MICROPYTHON=$PWD
    cd $MICROPYTHON
    git clone https://github.com/espressif/esp-idf.git
    cd esp-idf
    git checkout 5c88c5996dbde6208e3bec05abc21ff6cd822d26
    git submodule update --init --recursive

# Micropython
    cd $MICROPYTHON
    git clone https://github.com/tsaarni/micropython-with-esp32-cam.git
    -rename folder to micropython
    cd micropython
    git submodule update --init

# Camera for micropython
    cd $MICROPYTHON/esp-idf/components
    git clone https://github.com/tsaarni/esp32-camera-for-micropython.git
    -rename esp32-camera-for-micorpython to esp-camera
    export ESPIDF=$MICROPYTHON/esp-idf


# Make file change
    in micropython/ports/esp32/makefile - change in makefile on line 17 change PORT to /dev/tty.usbserial-A50285BI

# Envs
    export MICROPYTHON=$PWD
    export PATH=$PATH:$MICROPYTHON/xtensa-esp32-elf/bin
    export ESPIDF=$MICROPYTHON/esp-idf
    export AMPY_PORT=/dev/tty.usbserial-A50285BI
    
# Make flash
    cd $MICROPYTHON/micropython/ports/esp32
    -- may need -- make clean
    make V=1 SDKCONFIG=boards/sdkconfig.esp32cam -j

# Flash esp32
    esptool.py --port /dev/tty.usbserial-A50285BI --baud 115200 erase_flash
    -Switch to flash mode
    cd $MICROPYTHON/micropython/ports/esp32
    make deploy


## Installation
    cd $MICROPYTHON/arduino-esp32/ports/esp32
    make deploy
