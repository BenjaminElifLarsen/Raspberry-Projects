from digi.xbee.devices import XBeeDevice


device = XBeeDevice("/dev/ttyUSB0", 9600)

device.open()

for n in range(50):
  device.send_data_broadcast("Test")

device.close()
