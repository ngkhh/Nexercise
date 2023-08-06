#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <LedControl.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* firebaseAuth = "your_firebase_auth_token";

// Pins for dot matrix displays
const int dataPin = D7;
const int clockPin = D6;
const int csPin = D5;

// LedControl library object
LedControl lc = LedControl(dataPin, clockPin, csPin, 4);

// Game variables
const int numDisplays = 4;
const int numRounds = 10;
char alphabet;
int currentDisplay;
int roundCount;
int sessionNumber;

// Firebase variables
unsigned long sessionStartTime;
unsigned long lastActivityTime;
unsigned long userWakeTime;

// Inactivity variables
const unsigned long inactivityThreshold = 2400000; // 40 minutes (40 min * 60 sec * 1000 ms)
const unsigned long sleepThreshold = 3000000;     // 50 minutes (50 min * 60 sec * 1000 ms)

// Bad user record variables
const unsigned long badUserRecordThreshold = 2700000; // 45 minutes (45 min * 60 sec * 1000 ms)
bool badUserRecordFlag = false;

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  Serial.begin(115200);

  // Initialize dot matrix displays
  lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear displays

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup Firebase
  setupFirebase();

  // Start a new session
  startNewSession();
}

void startNewSession() {
  roundCount = 0;
  sessionStartTime = millis();
  sessionNumber = generateSessionNumber();
  Serial.print("Starting session number: ");
  Serial.println(sessionNumber);

  // Main loop
  while (true) {
    // Check for user input
    checkUserInput();

    // Check for inactivity and bad user record
    checkInactivity();
  }
}

void checkUserInput() {
  if (digitalRead(D1) == LOW) {
    handleUserInput('W');
  } else if (digitalRead(D2) == LOW) {
    handleUserInput('M');
  }
}

void handleUserInput(char userInput) {
  if (userInput == alphabet) {
    Serial.println("Correct!");
    recordUserResponse(userInput);
    playNextRound();
  } else {
    Serial.println("Wrong! Restarting the game...");
    recordUserResponse(userInput);
    recordBadUserRecord();
    startNewSession();
  }
}

void recordDisplayedAlphabet(char displayedAlphabet) {
  // Record displayed alphabet in Firebase (session number, displayed alphabet)
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/displayed_alphabet").c_str(), String(displayedAlphabet).c_str());
}

void recordLastActivityTime() {
  // Record last activity time in Firebase (session number, last activity time)
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/last_activity_time").c_str(), String(lastActivityTime).c_str());
}

void recordUserWakeTime() {
  // Record user wake time in Firebase (session number, user wake time)
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/user_wake_time").c_str(), String(userWakeTime).c_str());
}

void recordBadUserRecord() {
  // Record bad user record in Firebase (session number, bad user record timestamp)
  String path = "sessions/" + String(sessionNumber);
  unsigned long badUserRecordTime = millis();
  Firebase.setString(fbdo, (path + "/bad_user_record").c_str(), String(badUserRecordTime).c_str());
}

void playNextRound() {
  if (roundCount >= numRounds) {
    // All rounds completed for the session
    Serial.println("Game completed! Starting a new session");
    startNewSession();
    return;
  }

  roundCount++;
  alphabet = randomAlphabet();
  currentDisplay = randomDisplay();
  displayAlphabet(alphabet, currentDisplay);
  recordDisplayedAlphabet(alphabet);

  Serial.print("Round ");
  Serial.print(roundCount);
  Serial.print(" - Display: ");
  Serial.print(currentDisplay);
  Serial.print(" | Alphabet: ");
  Serial.println(alphabet);
}

// Rest of your code...

void enterSleepMode() {
  Serial.println("Entering sleep mode");
  
  lc.shutdown(0, true); // Turn off displays
  
  delay(100); // Wait for displays to settle

  // Put the ESP8266 into deep sleep mode
  ESP.deepSleep(sleepThreshold - inactivityThreshold, WAKE_RF_DEFAULT);
}

void exitSleepMode() {
  Serial.println("Exiting sleep mode");

  lc.shutdown(0, false); // Turn on displays
  delay(100); // Wait for displays to initialize
  
  // Record user wake time
  userWakeTime = millis();
  recordUserWakeTime();
}

void loop() {
  // Empty loop for now
}
