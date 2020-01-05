1. virtualenv -p python3 venv . venv/bin/activate

2. pip install pyserial pyparsing

3. git clone https://github.com/espressif/esp-idf.git
4. cd esp-idf
5. git checkout 5c88c5996dbde6208e3bec05abc21ff6cd822d26
6. git submodule update --init --recursive



7. cd $MICROPYTHON/esp-idf/components
8. git clone https://github.com/tsaarni/esp32-camera-for-micropython.git
    rename to esp32-camera

* ampy -->
        ~ Add env path -->
            export MICROPYTHON=/Users/eduardjacobs/Desktop/mciropython /->/ (shell needs a path)

        ~ Shell connection -->
            miniterm.py /dev/tty.usbserial-A50285BI 115200 --dtr 0 /->/ (press reset bttn)

        ~ Upload file to esp -->
            ampy --port /dev/tty.usbserial-A50285BI put boot.py
        
        ~ Run the file in esp -->
            ampy --port /dev/tty.usbserial-A50285BI run webcam.py


cd $MICROPYTHON/micropython/ports/esp32
make V=1 SDKCONFIG=boards/sdkconfig.esp32cam -j 

export MICROPYTHON=$PWD
export ESPIDF=~/Desktop/py/esp-idf
export PATH=$PATH:~/Desktop/py/xtensa-esp32-elf/bin
export AMPY_PORT=/dev/tty.usbserial-A50285BI