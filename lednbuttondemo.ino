#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi settings
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// MQTT settings
const char* mqtt_server = "your_MQTT_server_address";
const char* mqtt_username = "your_MQTT_username";
const char* mqtt_password = "your_MQTT_password";
const char* mqtt_topic = "your_MQTT_topic";

// Pin definitions
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define LED_PIN 4

// Create a Wi-Fi client object
WiFiClient espClient;

// Create a MQTT client object
PubSubClient mqttClient(espClient);

void setup() {
  // Set up the button pins as inputs with pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  // Set up the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Start serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Connect to MQTT server
  mqttClient.setServer(mqtt_server, 1883);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP8266_Client", mqtt_username, mqtt_password)) {
      mqttClient.subscribe(mqtt_topic);
    } else {
      delay(1000);
    }
  }
}

void loop() {
  // Check for button 1 press
  if (digitalRead(BUTTON1_PIN) == LOW) {
    // Send message to turn on LED
    mqttClient.publish(mqtt_topic, "button1_on");
    Serial.println("Button 1 is pressed - LED is on");
  }

  // Check for button 2 press
  if (digitalRead(BUTTON2_PIN) == LOW) {
    // Send message to turn off LED
    mqttClient.publish(mqtt_topic, "button2_on");
    Serial.println("Button 2 is pressed - LED is off");
  }

  // Check for messages from MQTT server
  mqttClient.loop();
}

// Callback function for handling MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Handle message to turn on LED
  if (message == "button1_on") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED is on");
  }

  // Handle message to turn off LED
  if (message == "button2_on") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED is off");
  }
}

//ur mom gay
