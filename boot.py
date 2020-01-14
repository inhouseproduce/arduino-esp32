import machine
import sht31

i = machine.I2C(sda=machine.Pin(5), scl=machine.Pin(4))
s = sht31.SHT31(i)
