#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

FirebaseData fbdo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* firebaseAuth = "your_firebase_auth_token";

// Pins for controller buttons and buzzer
const int buttonWPIN = 5;
const int buttonMPIN = 4;
const int buzzerPin = 15;

// Game variables
char userInput;
char displayedAlphabet;
unsigned long sessionStartTime;
unsigned long lastActivityTime;
unsigned int badRecords;

// Inactivity variables
const unsigned long inactivityThreshold = 2400000; // 40 minutes (40 min * 60 sec * 1000 ms)
bool isSleepMode = false;

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  Serial.begin(115200);

  // Start NTPClient
  timeClient.begin();
  timeClient.setTimeOffset(28800); // UTC +8 timezone
  
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
  displayedAlphabet = '-';
  badRecords = 0;
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
    
    // Check if user input matches displayed alphabet
    if (userInput == displayedAlphabet) {
      // If correct, play a short buzz sound
      digitalWrite(buzzerPin, HIGH);
      delay(200);
      digitalWrite(buzzerPin, LOW);
      // Proceed to next round
      startNewSession();
    } else {
      // If incorrect, play a long buzz sound and increment badRecords
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
      badRecords++;
    }
  }
}

void recordSessionData() {
  timeClient.update();
  String path = "sessions/" + String(sessionStartTime);
  Firebase.setString(fbdo, (path + "/start_time").c_str(), timeClient.getFormattedTime().c_str());
  Firebase.setString(fbdo, (path + "/end_time").c_str(), timeClient.getFormattedTime().c_str());
  Firebase.setString(fbdo, (path + "/user_input").c_str(), String(userInput).c_str());
  Firebase.setInt(fbdo, (path + "/bad_records").c_str(), badRecords);
}

void checkInactivity() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastActivityTime >= inactivityThreshold && !isSleepMode) {
    Serial.println("Inactivity detected, starting a new round");
    startNewSession();
  }
}

void loop() {
  timeClient.update();
  checkUserInput();
  checkInactivity();
}
