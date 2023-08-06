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
    displayTick();
    delay(1000); // Display tick for 1 second
    playNextRound();
  } else {
    Serial.println("Wrong! Restarting the game...");
    recordUserResponse(userInput);
    displayCross();
    delay(1000); // Display cross for 1 second
    recordBadUserRecord();
    startNewSession();
  }
}

void recordDisplayedAlphabet(char displayedAlphabet) {
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/displayed_alphabet").c_str(), String(displayedAlphabet).c_str());
}

void recordLastActivityTime() {
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/last_activity_time").c_str(), String(lastActivityTime).c_str());
}

void recordUserWakeTime() {
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/user_wake_time").c_str(), String(userWakeTime).c_str());
}

void recordBadUserRecord() {
  String path = "sessions/" + String(sessionNumber);
  unsigned long badUserRecordTime = millis();
  Firebase.setString(fbdo, (path + "/bad_user_record").c_str(), String(badUserRecordTime).c_str());
}

void displayTick() {
  byte tickPattern[8] = {
    B00000000,
    B00000001,
    B00000010,
    B00010100,
    B00001000,
    B00010000,
    B00000000,
    B00000000
  };

  for (int i = 0; i < numDisplays; i++) {
    lc.setRow(0, i, tickPattern[i]);
  }
}

void displayCross() {
  byte crossPattern[8] = {
    B00010000,
    B00001000,
    B00000100,
    B00000010,
    B00000001,
    B00000010,
    B00000100,
    B00001000
  };

  for (int i = 0; i < numDisplays; i++) {
    lc.setRow(0, i, crossPattern[i]);
  }
}

void playNextRound() {
  if (roundCount >= numRounds) {
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

char randomAlphabet() {
  char alphabets[] = {'M', 'W'};
  int index = random(0, 2);
  return alphabets[index];
}

int randomDisplay() {
  return random(0, numDisplays);
}

void displayAlphabet(char alphabet, int displayIndex) {
  lc.clearDisplay(0); // Clear all displays

  // Display the selected alphabet on the chosen display
  if (alphabet == 'M') {
    lc.setRow(0, displayIndex, B10000001);
    lc.setRow(0, displayIndex + 1, B11000001);
    lc.setRow(0, displayIndex + 2, B10100001);
    lc.setRow(0, displayIndex + 3, B10010001);
    lc.setRow(0, displayIndex + 4, B10000001);
  } else if (alphabet == 'W') {
    lc.setRow(0, displayIndex, B10000001);
    lc.setRow(0, displayIndex + 1, B10000001);
    lc.setRow(0, displayIndex + 2, B10101001);
    lc.setRow(0, displayIndex + 3, B10101001);
    lc.setRow(0, displayIndex + 4, B11000101);
  }
}

void checkInactivity() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastActivityTime >= inactivityThreshold) {
    playBuzzerSound();
    lastActivityTime = currentMillis;
    recordLastActivityTime();
  }

  if (!badUserRecordFlag && currentMillis - lastActivityTime >= badUserRecordThreshold) {
    Serial.println("Recording bad user record");
    badUserRecordFlag = true;
    recordBadUserRecord();
  }

  if (currentMillis - lastActivityTime >= sleepThreshold) {
    Serial.println("Entering sleep mode");
    delay(500);
    lc.shutdown(0, true); // Turn off displays
    delay(1000);
    lc.shutdown(0, false); // Turn on displays
    Serial.println("Exiting sleep mode");
  }
}

void loop() {
  // Empty loop for now
}
