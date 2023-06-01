#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

const char* mqttServer = "MQTTServerIPAddress";
const int mqttPort = 1883;
const char* mqttUser = "MQTTUsername";
const char* mqttPassword = "MQTTPassword";
const char* mqttTopic = "led/control";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const int onButtonPin = D2;  // GPIO pin for "ON" button
const int offButtonPin = D3; // GPIO pin for "OFF" button

void setup() {
  pinMode(onButtonPin, INPUT_PULLUP);
  pinMode(offButtonPin, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Connect to MQTT Broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("InputESP8266", mqttUser, mqttPassword)) {
      client.subscribe(mqttTopic);
    }
    delay(1000);
  }
}

void loop() {
  client.loop();

  // Check if "ON" button is pressed
  if (digitalRead(onButtonPin) == LOW) {
    client.publish(mqttTopic, "ON");
    delay(500); // Debounce delay
  }

  // Check if "OFF" button is pressed
  if (digitalRead(offButtonPin) == LOW) {
    client.publish(mqttTopic, "OFF");
    delay(500); // Debounce delay
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if needed
}
