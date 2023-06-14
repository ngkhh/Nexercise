#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <PubSubClient.h>
#include <LedControl.h>

// Constants for Wi-Fi connection
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// Firebase configuration
const char* firebaseHost = "your-firebase-host.firebaseio.com";
const char* firebaseAuth = "your-firebase-authentication";

// MQTT configuration
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqttPort = 1883;
const char* mqttTopic = "nexercise/control";

// Pins for dot matrix displays
const int numDisplays = 4;
const int displayDataPin = 2;
const int displayClockPin = 3;
const int displayLatchPin = 4;

// Pins for controller buttons
const int buttonWPIN = 6;  // Example pin number, modify as per your setup
const int buttonMPIN = 7;  // Example pin number, modify as per your setup

// Game variables
char alphabet;
int currentDisplay;
int roundCount;

// Firebase variables
unsigned long sessionStartTime;

// MQTT variables
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// LedControl object for dot matrix displays
LedControl lc = LedControl(displayDataPin, displayClockPin, displayLatchPin, numDisplays);

void setup() {
  // Initialize dot matrix displays
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // Initialize controller buttons
  pinMode(buttonWPIN, INPUT_PULLUP);
  pinMode(buttonMPIN, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);

  // Initialize MQTT
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);

  // Connect to MQTT broker
  connectToMqttBroker();

  // Start a new session
  startNewSession();
}

void loop() {
  // Check for user input
  if (digitalRead(buttonWPIN) == LOW) {
    checkUserInput('W');
  } else if (digitalRead(buttonMPIN) == LOW) {
    checkUserInput('M');
  }

  // Check for inactivity and remind the user to continue the session
  if ((millis() - sessionStartTime) > 1800000) {  // 30 minutes (30 min * 60 sec * 1000 ms)
    playBuzzerSound();
  }

  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    reconnectToMqttBroker();
  }
  mqttClient.loop();
}

void startNewSession() {
  roundCount = 0;
  sessionStartTime = millis();
  Serial.println("Starting a new session");

  playNextRound();
}

void playNextRound() {
  roundCount++;
  Serial.print("Round ");
  Serial.println(roundCount);

  // Randomly select a dot matrix display
  currentDisplay = random(numDisplays);

  // Generate a random alphabet ('M' or 'W')
  alphabet = random(2) == 0 ? 'M' : 'W';

  // Turn on the selected dot matrix display and show the alphabet
  lc.clearDisplay(0);
  lc.setChar(0, currentDisplay, alphabet);

  // Publish the alphabet to the MQTT topic
  mqttClient.publish(mqttTopic, String(alphabet).c_str());

  Serial.print("Displaying ");
  Serial.print(alphabet);
  Serial.print(" on display ");
  Serial.println(currentDisplay);
}

void checkUserInput(char userInput) {
  if (userInput == alphabet) {
    // Correct input, move to the next round
    playNextRound();
  } else {
    // Incorrect input, restart the game
    Serial.println("Incorrect input. Restarting game...");
    logSessionData();
    startNewSession();
  }
}

void logSessionData() {
  // Log session data to Firebase (start time, end time, displayed alphabet, pressed button)
  unsigned long sessionEndTime = millis();
  Firebase.pushInt("sessions/start_time", sessionStartTime);
  Firebase.pushInt("sessions/end_time", sessionEndTime);
  Firebase.pushString("sessions/displayed_alphabet", String(alphabet));
  Firebase.pushString("sessions/pressed_button", String(alphabet));
}

void displayAlphabet(char alphabet) {
  // Code for displaying the alphabet on the dot matrix display
  lc.setChar(0, currentDisplay, alphabet);
}

void playBuzzerSound() {
  // Code for playing the buzzer sound to remind the user
  // Modify this function based on your buzzer library and hardware
  Serial.println("Playing buzzer sound");
}

void connectToMqttBroker() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (mqttClient.connect("NexerciseClient")) {
      Serial.println("Connected to MQTT broker");
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(1000);
    }
  }
}

void reconnectToMqttBroker() {
  while (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    if (mqttClient.connect("NexerciseClient")) {
      Serial.println("Reconnected to MQTT broker");
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(1000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages (if needed)
}
