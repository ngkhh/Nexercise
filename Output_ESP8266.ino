#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

const char* mqttServer = "597c9bc0b8d0400b8f723ca742f37e73.s2.eu.hivemq.cloud";
const int mqttPort = 1883;
const char* mqttUser = "hivemq.webclient.1685159104447";
const char* mqttPassword = "8U:B<p>@3mnI7Af2QLot";
const char* mqttTopic = "led/control";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const int ledPin = D4; // GPIO pin for LED

void setup() {
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Connect to MQTT Broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("OutputESP8266", mqttUser, mqttPassword)) {
      client.subscribe(mqttTopic);
    }
    delay(1000);
  }
}

void loop() {
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, mqttTopic) == 0) {
    String message = "";
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }

    // Turn on LED if message is "ON"
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);
    }

    // Turn off LED if message is "OFF"
    if (message == "OFF") {
      digitalWrite(ledPin, LOW);
    }
  }
}
