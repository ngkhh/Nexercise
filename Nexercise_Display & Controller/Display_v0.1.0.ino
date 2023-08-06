#include <Firebase.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <LedControl.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "CLPHS_Admin";
const char* password = "smclp8283655";

// Firebase configuration
const char* firebaseHost = "https://nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app/"; // Update with your Firebase project URL
const char* firebaseAuth = "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4"; // Update with your Firebase project auth token

// Pins for dot matrix displays
const int dataPin = 14;
const int clockPin = 12;
const int csPin = 13;

// Sleep mode parameters
const unsigned long sleepTimeout = 3000000; // 50 minutes (50 min * 60 sec * 1000 ms)
unsigned long lastActivityTime = 0;
unsigned long userWakeTime = 0; // Variable to store user wake screen timestamp

LedControl lc = LedControl(dataPin, clockPin, csPin, 1); // Initialize LedControl object

// User input pin for waking up from sleep mode
const int userInputPin = 4; // Example pin for user input

// Game variables
const int numDisplays = 4;
const int numRounds = 10;
char alphabet;
int currentDisplay;
int roundCount;
int sessionNumber;

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

  // Initialize user input pin
  pinMode(userInputPin, INPUT_PULLUP);

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

void loop() {
  // Check for incoming messages from Firebase
  if (Firebase.ready()) {
    Firebase.getString(fbdo, "displayMessage");
    if (fbdo.dataAvailable()) {
      String receivedMessage = fbdo.stringData();
      displayMessage(receivedMessage);
      lastActivityTime = millis(); // Update activity time
      userWakeTime = lastActivityTime; // Update user wake screen timestamp
      updateFirebaseActivityTime(); // Update last activity time and user wake screen timestamp in Firebase
    }
  }

  // Check for user inactivity to enter sleep mode
  if (millis() - lastActivityTime >= sleepTimeout) {
    enterSleepMode();
  }

  // Check for session completion
  if (roundCount >= numRounds) {
    // All rounds completed for the session
    Serial.println("Session completed! Starting a new session");
    recordSessionEnd();
    startNewSession();
  }
}

void displayMessage(String message) {
  if (message == "M" || message == "W") {
    char charToDisplay = message.charAt(0);
    displayChar(charToDisplay);
    lastActivityTime = millis(); // Update activity time
    userWakeTime = lastActivityTime; // Update user wake screen timestamp
    updateFirebaseActivityTime(); // Update last activity time and user wake screen timestamp in Firebase
  }
}

void displayChar(char c) {
  // Define your character patterns here (e.g., M and W patterns)
  byte charM[8] = {B10000001, B10010001, B10101001, B11000101, B10000001, B10000001, B10000001, B10000001};
  byte charW[8] = {B10000001, B10000001, B10000001, B10011001, B10100101, B11000001, B10000001, B10000001};

  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, c == 'M' ? charM[i] : charW[i]);
  }
}

void enterSleepMode() {
  // Put the display module into sleep mode here
  // For example, you can turn off displays and other peripherals
  lc.shutdown(0, true); // Turn off displays
  delay(100);           // Wait for displays to turn off
  ESP.deepSleep(sleepTimeout * 1000); // Enter deep sleep mode for sleepTimeout seconds
}

void updateFirebaseActivityTime() {
  // Update last activity time and user wake screen timestamp in Firebase
  String path = "displayModule";
  Firebase.setString(fbdo, (path + "/lastActivityTime").c_str(), String(lastActivityTime).c_str());
  Firebase.setString(fbdo, (path + "/userWakeTime").c_str(), String(userWakeTime).c_str());
}

void startNewSession() {
  roundCount = 0;
  sessionNumber = generateSessionNumber();
  Serial.print("Starting session number: ");
  Serial.println(sessionNumber);

  recordSessionStart();
  playNextRound();
}

void playNextRound() {
  if (roundCount >= numRounds) {
    return;
  }

  roundCount++;
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

void displayAlphabet(char alphabet, int displayIndex) {
  lc.clearDisplay(0);             // Clear all displays
  lc.setChar(0, displayIndex, alphabet, false); // Display the alphabet on the selected display
}

void recordSessionStart() {
  // Record session start time in Firebase
  unsigned long sessionStartTime = millis();
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/start_time").c_str(), String(sessionStartTime).c_str());
}

void recordSessionEnd() {
  // Record session end time in Firebase
  unsigned long sessionEndTime = millis();
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/end_time").c_str(), String(sessionEndTime).c_str());
}

int generateSessionNumber() {
  // Generate a random session number between 1 and 1000
  return random(1, 1001);
}
