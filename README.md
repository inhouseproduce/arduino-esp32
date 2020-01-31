## Development

## Quick start
    dependencies --> python3 setup.sh
    build software --> python3 build.sh
    make file --> python3 make.sh -> change makefile
    flash esp --> python3 flash_esp.sh
    ampy --port /dev/tty.usbserial-A50285BI put boot.py

# Erase flash
    esptool.py --port /dev/tty.usbserial-A50285BI --baud 115200 erase_flash

# Envs
    export MICROPYTHON=$PWD
    export PATH=$PATH:$MICROPYTHON/xtensa-esp32-elf/bin
    export ESPIDF=$MICROPYTHON/esp-idf
    export AMPY_PORT=/dev/tty.usbserial-A50285BI
    
# Flash esp32
    cd $MICROPYTHON/micropython/ports/esp32
    make V=1 SDKCONFIG=boards/sdkconfig.esp32cam -j
    make deploy
    
# Push file to esp
    ampy --port /dev/tty.usbserial-A50285BI put boot.py

# Run the file (Local)
    ampy --port /dev/tty.usbserial-A50285BI run boot.py

# Direct shell
    miniterm.py /dev/tty.usbserial-A50285BI 115200 --dtr 0
    -- Press restart to boot

# Install micropython local libary (upip)
    -- write in the terminal (loads to local system /libs)
    import upip
    upip.install('ujson')



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
