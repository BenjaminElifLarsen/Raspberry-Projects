import time
import paho.mqtt.client as paho
from paho import mqtt

from digi.xbee.devices import XBeeDevice


# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

client = paho.Client(client_id="Raspberry Pi", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect


client.on_message = on_message
client.on_publish = on_publish




device = XBeeDevice("/dev/ttyUSB0", 9600)

device.open()
while(True): 
    message = device.read_data()
    if message != None: 
        data = message.data
        remote = message.remote_device.get_64bit_addr().address.hex()
        print("Remote: ")
        print(remote)
        if remote == "0013a2004106f9fc":
            mes = data.decode()
            print("PERMITTED")
            print(mes)
            client.connect("localhost",1883)
            client.publish("temp", payload=mes, qos=1)
            client.disconnect()
        else:
            print("NOT PERMITTED")
device.close()
