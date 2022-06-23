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

# print which topic was subscribed to
#def on_subscribe(client, userdata, mid, granted_qos, properties=None):
#    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

client = paho.Client(client_id="Raspberry Pi", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

#client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)


#client.connect("localhost", 1883)

#client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

#client.subscribe("nej", qos=1)

#client.publish("nej", payload="Yep, nej is neat", qos=1)


device = XBeeDevice("/dev/ttyUSB0", 9600)

device.open()
#take any data from the XBee and transmit to the mqtt
while(True): 
    message = device.read_data()
    if message != None: #check if the message is from a trusted MAC.
        data = message.data
        remote = message.remote_device
        print("Remote: ")
        print(remote)
        if remote == "0013A2004106F9FC":
            mes = data.decode()
            print("Got data: ")
            print(mes)
            client.connect("localhost",1883)
            client.publish("temp", payload=mes, qos=1)
            client.disconnect()
        else:
            print("NOT PERMITTED")
device.close()
