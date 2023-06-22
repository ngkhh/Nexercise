#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LedControl.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420.firebaseapp.com";
const char* firebaseAuth = "your_firebase_auth_key";

// MQTT configuration
const char* mqttBroker = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttClientId = "mqtt_client_id";
const char* mqttUsername = "mqtt_username";
const char* mqttPassword = "mqtt_password";
const char* mqttTopic = "nexercise/project";

// Pins for dot matrix displays
const int dataPin = D5;
const int clockPin = D6;
const int csPin = D7;

// Pins for controller buttons
const int buttonWPIN = D1;
const int buttonMPIN = D2;

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

// Buzzer pin
const int buzzerPin = D8;

// Inactivity variables
unsigned long lastActivityTime = 0;
const unsigned long inactivityThreshold = 1800000; // 30 minutes (30 min * 60 sec * 1000 ms)
const unsigned long sleepThreshold = 2400000;     // 40 minutes (40 min * 60 sec * 1000 ms)

// Flag to indicate if the device is in sleep mode
bool isSleepMode = false;

// Flag to indicate if the display should be updated
bool shouldUpdateDisplay = true;

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  // Initialize dot matrix displays
  lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear displays

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

  // Setup OTA (Over-The-Air) updates
  setupOTA();

  // Start a new session
  startNewSession();
}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();

  // Check for user input
  if (!isSleepMode) {
    checkUserInput();
  }

  // Check for inactivity and remind the user to continue the session
  checkInactivity();

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

void checkUserInput() {
  if (digitalRead(buttonWPIN) == LOW) {
    handleUserInput('W');
  } else if (digitalRead(buttonMPIN) == LOW) {
    handleUserInput('M');
  }
}

void handleUserInput(char userInput) {
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
  lc.clearDisplay(0);             // Clear all displays
  lc.setChar(0, displayIndex, alphabet, false); // Display the alphabet on the selected display
}

void playBuzzerSound() {
  // Example implementation for playing the buzzer sound
  // Modify this function based on your specific buzzer library and hardware
  // Add code here to generate the buzzer sound

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

void checkInactivity() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastActivityTime >= inactivityThreshold && !isSleepMode) {
    // User is inactive for the specified threshold
    playBuzzerSound();
    lastActivityTime = currentMillis;
  }

  if (currentMillis - lastActivityTime >= sleepThreshold && !isSleepMode) {
    // User is inactive for sleep threshold, enter sleep mode
    Serial.println("Entering sleep mode...");
    isSleepMode = true;
    lc.shutdown(0, true); // Shutdown the displays
    ESP.deepSleep(0);
  }
}

void setupOTA() {
  // Configure OTA parameters
  ArduinoOTA.setHostname("nexercise_esp");  // Set the hostname prefix for OTA updates
  ArduinoOTA.setPassword("ota_password");  // Set the OTA password (optional)

  // Setup OTA handlers
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }

    // NOTE: When updating the sketch, the device will be reset automatically
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");

    // Delay to ensure the OTA process is completed
    delay(500);

    // Reinitialize Firebase after OTA
    setupFirebase();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  // Start OTA server
  ArduinoOTA.begin();
}
