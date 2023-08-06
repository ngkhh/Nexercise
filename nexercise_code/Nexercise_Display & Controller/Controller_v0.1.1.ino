#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* firebaseAuth = "your_firebase_auth_token";

// Pins for controller buttons and buzzer
const int buttonWPIN = D1;
const int buttonMPIN = D2;
const int buzzerPin = D3;

// Game variables
char userInput;
unsigned long sessionStartTime;
unsigned long lastActivityTime;
unsigned long userWakeTime;

// Inactivity variables
const unsigned long inactivityThreshold = 2400000; // 40 minutes (40 min * 60 sec * 1000 ms)
const unsigned long sleepThreshold = 3000000;     // 50 minutes (50 min * 60 sec * 1000 ms)
bool isSleepMode = false;

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup Firebase
  setupFirebase();

  // Setup buzzer pin
  pinMode(buzzerPin, OUTPUT);

  // Start a new session
  startNewSession();
}

void startNewSession() {
  sessionStartTime = millis();
  Serial.println("Starting a new session");
  userInput = '-';
}

void checkUserInput() {
  if (digitalRead(buttonWPIN) == LOW) {
    handleUserInput('W');
  } else if (digitalRead(buttonMPIN) == LOW) {
    handleUserInput('M');
  }
}

void handleUserInput(char input) {
  if (input == 'W' || input == 'M') {
    userInput = input;
    lastActivityTime = millis();
  }
}

void recordSessionData() {
  String path = "sessions/" + String(sessionStartTime);
  Firebase.setString(fbdo, (path + "/start_time").c_str(), String(sessionStartTime).c_str());
  Firebase.setString(fbdo, (path + "/end_time").c_str(), String(millis()).c_str());
  Firebase.setString(fbdo, (path + "/user_input").c_str(), String(userInput).c_str());
}

void checkInactivity() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastActivityTime >= inactivityThreshold && !isSleepMode) {
    Serial.println("Inactivity detected, starting a new round");
    startNewSession();
  }

  if (currentMillis - lastActivityTime >= 2400000 && currentMillis - lastActivityTime < inactivityThreshold && !isSleepMode) {
    // Play buzzer sound for alert after 40 minutes of inactivity
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
  }

  if (currentMillis - lastActivityTime >= sleepThreshold && !isSleepMode) {
    Serial.println("Entering sleep mode");
    isSleepMode = true;
    // Add code here to put the device into sleep mode
    // For example, you can turn off the controller components to conserve power
    userWakeTime = millis();
    recordSessionData();
    // Add code here to wake up the device from sleep mode
    Serial.println("Exiting sleep mode");
    isSleepMode = false;
  }
}

void loop() {
  checkUserInput();
  checkInactivity();
}
