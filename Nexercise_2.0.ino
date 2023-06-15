#include <Firebase.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LedControl.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420.firebaseapp.com";
const char* firebaseAuth = "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4";

// MQTT configuration
const char* mqttBroker = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttClientId = "mqttx_22b19574";
const char* mqttUsername = "emqx";
const char* mqttPassword = "public";
const char* mqttTopic = "nexercise/project";

// Pins for dot matrix displays
const int dataPin = D5;  // Example pin number, modify as per your setup
const int clockPin = D6; // Example pin number, modify as per your setup
const int csPin = D7;    // Example pin number, modify as per your setup

// Pins for controller buttons
const int buttonWPIN = D1; // Example pin number, modify as per your setup
const int buttonMPIN = D2; // Example pin number, modify as per your setup

// Game variables
const int numDisplays = 4;
const int numRounds = 10;
char alphabet;
int currentDisplay;
int roundCount;

// Firebase variables
unsigned long sessionStartTime;

// MQTT client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// LedControl library object
LedControl lc = LedControl(dataPin, clockPin, csPin, numDisplays);

void setupFirebase(){
// Initialize Firebase
  Firebase.begin("nexercise-69420.firebaseapp.com", "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4");
}

void setup() {
  // Initialize dot matrix displays
  lc.shutdown(0, false);   // Wake up displays
  lc.setIntensity(0, 8);   // Set brightness (0-15)
  lc.clearDisplay(0);      // Clear displays

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

  // Connect to MQTT broker
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);
  connectToMqtt();

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

  // Handle MQTT messages
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
  if (roundCount > numRounds) {
    Serial.println("Game completed! Starting a new session");
    recordSessionData('-', '-'); // Record session data for completed game
    startNewSession();
    return;
  }

  alphabet = randomAlphabet();
  currentDisplay = randomDisplay();
  displayAlphabet(alphabet, currentDisplay);

  Serial.print("Round ");
  Serial.print(roundCount);
  Serial.print(" - Display: ");
  Serial.print(currentDisplay);
  Serial.print(" | Alphabet: ");
  Serial.println(alphabet);
}

char randomAlphabet() {
  char alphabets[] = {'M', 'W'};
  int index = random(0, 2);
  return alphabets[index];
}

int randomDisplay() {
  return random(0, numDisplays);
}

void checkUserInput(char userInput) {
  if (userInput == alphabet) {
    Serial.println("Correct!");
    playNextRound();
  } else {
    Serial.println("Wrong! Restarting the game...");
    recordSessionData(alphabet, userInput);
    startNewSession();
  }
}

void recordSessionData(char displayedAlphabet, char pressedButton) {
  // Record session data in Firebase (start time, end time, displayed alphabet, pressed button)
  unsigned long sessionEndTime = millis();
  Firebase.pushInt(fbdo, "sessions/start_time", sessionStartTime);
  Firebase.pushInt(fbdo, "sessions/end_time", sessionEndTime);
  Firebase.pushString(fbdo, "sessions/displayed_alphabet", String(displayedAlphabet));
  Firebase.pushString(fbdo, "sessions/pressed_button", String(pressedButton));
}

void displayAlphabet(char alphabet, int displayIndex) {
  lc.clearDisplay(0); // Clear all displays
  lc.setChar(0, displayIndex, alphabet, false); // Display the alphabet on the selected display
}

void playBuzzerSound() {
  // Example implementation for playing the buzzer sound
  // Modify this function based on your specific buzzer library and hardware
  // Add code here to generate the buzzer sound

  // Assuming you are using a buzzer connected to pin D8
  const int buzzerPin = D8;
  
  // Play the buzzer sound for 1 second
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void connectToMqtt() {
  // Connect to MQTT broker
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
      Serial.println("Connected to MQTT");
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(1000);
    }
  }
}

void publishMqttMessage(String message) {
  // Publish the MQTT message
  mqttClient.publish(mqttTopic, message.c_str());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages here
  // Modify this function based on your MQTT message handling logic
  // Add code here to handle the received message

  Serial.print("Received MQTT message on topic: ");
  Serial.println(topic);
  Serial.print("Message payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
