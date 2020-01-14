# ESP8266

### Connection Instructions
  Connect GPIO Pin 4 to scl
  Connect GPIO Pin 5 to sda

### Flash Board
    esptool.py --port /dev/ttyUSB0 erase_flash
    esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash --flash_size=detect 0 esp8266-lastUpdateLabel.bin

### Push file to esp
    ampy --port /dev/ttyUSB0 put boot.py
    ampy --port /dev/ttyUSB0 put main.py
    ampy --port /dev/ttyUSB0 put sht31.py

### Run the file (Local)
    ampy --port /dev/tty.usbserial-A50285BI run main.py

### Direct shell
    miniterm.py /dev/tty.usbserial-A50285BI 115200 --dtr 0
    -- Press restart to boot

### Install micropython local libary (upip)
    -- write in the terminal (loads to local system /libs)
    import upip
    upip.install('ujson')

### From Scratch
### Tools
    pip install pyserial
    pip install pyparsing
    pip install esptool
    pip install virtualenv
    pip install adafruit-ampy
    brew install cmake ninja
    brew install ccache
