import network

import esp
esp.osdebug(None)

import gc
gc.collect()

ssid = 'TP-Link_33C4'
password = 'nasturtium'

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(ssid, password)

while station.isconnected() == False:
  pass