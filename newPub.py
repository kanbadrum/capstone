import paho.mqtt.publish as publish
from datetime import datetime

# Define the broker address and port
broker_address = "192.168.43.133"
port = 1883

# PACKETS

packet = "0xFF"
# Define the topic and message

topic = "capstone"
message = packet

# Publish the message to the specified topic
publish.single(topic, message, hostname=broker_address, port=port)
