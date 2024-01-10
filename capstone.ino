#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define RST_PIN  13    // RFID RST
#define SS_PIN   5     // RFID SDA

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create an instance of the MFRC522 class
bool cardPresent = false;          // Flag to indicate if a card is present

// WiFi credentials
const char *ssid = "mqtt";
const char *password = "zzzzzzzz";

// MQTT broker settings
const char *mqttBroker = "192.168.43.133";
const char *mqttTopic = "capstone";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup(){
  Serial.begin(115200);

  connectToWiFi();
  connectToMQTT();

  // RFID CARD
  SPI.begin();        // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize the RC522 module
}

void loop() { // TODO: add buzzer sounds and/or led on events
  simulateSoftwareInterrupt();  // Check for card presence

  if (cardPresent) {
    rfid();
    cardPresent = false;  // Reset the flag
  }

  // Maintain the MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi: " + String(ssid));
  // TODO: print connecting to wifi on lcd
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // TODO: config cd time
    Serial.println("Connecting to WiFi: " + String(ssid));
    // TODO: print connecting to wifi on lcd
  }

  Serial.println("Connected to WiFi: " + String(ssid));
  // TODO: print connected to wifi on lcd
}

void connectToMQTT() {
  Serial.println("Connecting to MQTT broker: " + String(mqttBroker));
  // TODO: print connecting to mqtt on lcd

  mqttClient.setServer(mqttBroker, 1883);  // Set MQTT broker IP and port

  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker: " + String(mqttBroker));
      // TODO: print connected to mqtt on lcd
    } else {
      Serial.println("Failed to connect to MQTT broker. Retrying...");
      // TODO: print connecting to mqtt on lcd
      delay(1000); // TODO: config cd time
    }
  }
}

void reconnectMQTT() {
  Serial.println("MQTT connection lost. Reconnecting...");
  // TODO: print mqtt connection lost on lcd

  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Reconnected to MQTT broker: " + String(mqttBroker));
      // TODO: print connected to mqtt on lcd
    } else {
      Serial.println("Failed to reconnect to MQTT broker. Retrying...");
      // TODO: print connecting to mqtt on lcd
      delay(1000);  // Increase delay to give more time for reconnection
    }
  }
}

void simulateSoftwareInterrupt() {
  static unsigned long lastCheckTime = 0;
  unsigned long currentTime = millis();

  // Check for card presence every 1000 milliseconds
  if (currentTime - lastCheckTime >= 1000) {
    lastCheckTime = currentTime;

    if (mfrc522.PICC_IsNewCardPresent()) {
      cardPresent = true;
    }
  }
}

void rfid() {
  // Perform RFID card reading operation
  if (!mfrc522.PICC_ReadCardSerial()) {
    // Handle card reading failure
    Serial.println("Error reading card");
    // TODO: print error on lcd
    return;
  }
  
  // Declare an array to store the UID as uint8_t
  uint8_t uidBytes[4];

  // Copy UID bytes into the array
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidBytes[i] = mfrc522.uid.uidByte[i];
  }

  // Print UID bytes
  Serial.print("UID: ");
  for (byte i = 0; i < sizeof(uidBytes); i++) {
    Serial.print(uidBytes[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Call function to publish to MQTT
  publish(uidBytes);
}

void publish(uint8_t uid[]) {
  // TODO: print sending data on lcd

  // Pack data to be sent
  // CMD + deviceID + PYLD
  uint8_t cmd = 0x81;
  uint8_t deviceID = 0xf1; // Replace with your actual deviceID
  uint8_t pyld[6];

  pyld[0] = cmd;
  pyld[1] = deviceID;
  pyld[2] = uid[0];
  pyld[3] = uid[1];
  pyld[4] = uid[2];
  pyld[5] = uid[3];

  // Print the packed data in hexadecimal
  Serial.print("Original Data: ");
  for (uint8_t i = 0; i < sizeof(pyld); i++) {
    Serial.print(pyld[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  uint8_t key[6] = {0xAB, 0xCD, 0xEF, 0x98, 0x76, 0x54};
  uint8_t coded[6];

  for(uint8_t i = 0; i < 6; i++){
    coded[i] = pyld[i] ^ key[i];
  }

  Serial.print("Encrypted Data: ");
  for (uint8_t i = 0; i < sizeof(coded); i++) {
    Serial.print(coded[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

