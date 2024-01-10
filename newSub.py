import paho.mqtt.client as mqtt

# Callback when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    
    # Subscribe to the topic when connected
    client.subscribe("capstone")

# Callback when a message is received from the broker
def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()}")

# Create an MQTT client
client = mqtt.Client()

# Set the callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker
client.connect("192.168.43.133", 1883, 60)

# Start the loop to handle communication with the broker
client.loop_forever()
