import network

import esp
esp.osdebug(None)

import gc
gc.collect()

import test

ssid = 'edoanna'
password = 'edo111792'

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass