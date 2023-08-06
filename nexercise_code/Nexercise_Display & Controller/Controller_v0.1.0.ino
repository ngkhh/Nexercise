#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420.firebaseapp.com";
const char* firebaseAuth = "your_firebase_auth";

// MQTT configuration
const char* mqttBroker = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttClientId = "nexercise";
const char* mqttUsername = "steve";
const char* mqttPassword = "public";
const char* mqttTopic = "nexercise";

// Pins for buttons
const int buttonWPin = 5;
const int buttonMPin = 4;

// Buzzer pin
const int buzzerPin = 15;

// Time tracking variables
unsigned long sessionStartTime;
unsigned long lastResponseTime;

// Session timeout (in milliseconds)
const unsigned long sessionTimeout = 30 * 60 * 1000;   // 30 minutes
const unsigned long buzzerDuration = 3000;             // 3 seconds
const unsigned long responseTimeout = 5 * 60 * 1000;   // 5 minutes

// Variables to store user responses
char userResponse;
char expectedResponse;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setupWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setupMQTT() {
  // Set MQTT server and port
  mqttClient.setServer(mqttBroker, mqttPort);

  // Set MQTT callback function
  mqttClient.setCallback(mqttCallback);

  // Connect to MQTT broker
  if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
    Serial.println("Connected to MQTT broker");
    mqttClient.subscribe(mqttTopic);
  } else {
    Serial.println("MQTT connection failed");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  setupWiFi();

  // Setup Firebase
  setupFirebase();

  // Setup MQTT
  setupMQTT();

  // Setup OTA (Over-The-Air) update
  ArduinoOTA.begin();
  ArduinoOTA.setHostname("nexercise-controller");
}

void loop() {
  // Handle MQTT messages
  mqttClient.loop();

  // Check if a user session is active
  if (sessionStartTime != 0) {
    // Check for user response
    if (checkUserResponse()) {
      // User response is correct
      // Additional code to handle correct user response
      // ...

      // Reset session timer
      lastResponseTime = millis();
    } else if (millis() - lastResponseTime > responseTimeout) {
      // User did not respond within the timeout period
      triggerBuzzer();
      delay(5000);  // Wait for 5 minutes before sending data to Firebase and entering sleep mode
      sendBadUserRecord();
      enterSleepMode();
    }
  }

  // Additional code for your specific application logic
  // ...

  // OTA (Over-The-Air) update handling
  ArduinoOTA.handle();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle MQTT messages
  // Modify the following code to extract the expected response from the received MQTT message
  // Example: String mqttMessage = String((char*)payload);
  //          expectedResponse = getExpectedResponseFromMQTTMessage(mqttMessage);
}

bool checkUserResponse() {
  // Check if the user pressed the correct button (W or M)
  // Modify the following code based on your button handling logic
  // Example: if (digitalRead(buttonWPin) == HIGH && userResponse == 'W') {
  //              return true;
  //          }
  //          if (digitalRead(buttonMPin) == HIGH && userResponse == 'M') {
  //              return true;
  //          }
  //          return false;
}

void triggerBuzzer() {
  // Activate the buzzer for the specified duration
  unsigned long buzzerStartTime = millis();
  while (millis() - buzzerStartTime < buzzerDuration) {
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}

void sendBadUserRecord() {
  // Record the bad user data in Firebase
  // Modify the following code to send the bad user record to Firebase
  // Example: Firebase.pushString(fbdo, "bad_users", "Bad User Record");
}

void enterSleepMode() {
  // Enter sleep mode
  // Modify the following code to enter sleep mode
  // Example: ESP.deepSleep(5 * 60 * 1000, WAKE_RF_DEFAULT);
}
