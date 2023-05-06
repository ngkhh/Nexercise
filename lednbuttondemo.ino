#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// MQTT broker settings
const char* mqttServer = "597c9bc0b8d0400b8f723ca742f37e73.s2.eu.hivemq.cloud";
const int mqttPort = 8883;
const char* mqttUser = "testing";
const char* mqttPassword = "Taneelim2019";
const char* ledTopic = "testing";

// WiFi settings
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Pin definitions
const int buttonPin1 = 0;  // Change to your first button's pin
const int buttonPin2 = 2;  // Change to your second button's pin
const int ledPin = 16;     // Change to your LED's pin

WiFiClient espClient;
PubSubClient client(espClient);

bool button1State = false;
bool button2State = false;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT broker
  Serial.print("Connecting to MQTT broker ");
  Serial.print(mqttServer);
  Serial.print(":");
  Serial.println(mqttPort);
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(ledTopic);
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Handle MQTT messages
  if (client.connected()) {
    client.loop();
  } else {
    Serial.println("Disconnected from MQTT broker, reconnecting...");
    while (!client.connected()) {
      if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
        Serial.println("Connected to MQTT broker");
        client.subscribe(ledTopic);
      } else {
        Serial.print("Failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" retrying in 5 seconds");
        delay(5000);
      }
    }
  }

  // Handle button states
  if (digitalRead(buttonPin1) == LOW) {
    if (!button1State) {
      button1State = true;
      Serial.print("Button 1: ");
      Serial.print(button1State);
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      client.publish(ledTopic, ledState ? "1" : "0");
    }

  } else {
    button1State = false;
  }

  if (digitalRead(buttonPin2) == LOW) {
    if (!button2State) {
      button2State = true;
      Serial.println("Button 2 pressed");
      // Handle button 2 press here
    }
  } else {
    button2State = false;
  }

  // Print status to serial monitor
  Serial.print("Button 1: ");
  Serial.print(button1State);
  Serial.print(", Button 2: ");
  Serial.print(button2State);
  Serial.print(", LED: ");
  Serial.println(ledState);

  // Wait before checking button states again
  delay(100);
}
