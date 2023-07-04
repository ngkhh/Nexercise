#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <LedControl.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420.firebaseapp.com";
const char* firebaseAuth = "your_firebase_auth";

// Pins for dot matrix displays
const int dataPin = 14;
const int clockPin = 12;
const int csPin = 13;

// LedControl library object
LedControl lc = LedControl(dataPin, clockPin, csPin, 1);  // 1 for single display

// Buzzer pin
const int buzzerPin = 15;

// Firebase session variables
unsigned long sessionStartTime;
char displayedAlphabet;

void setupFirebase() {
  // Initialize Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  Serial.begin(115200);

  // Initialize dot matrix display
  lc.shutdown(0, false); // Wake up display
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear display

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup Firebase
  setupFirebase();
}

void loop() {
  // Check for incoming MQTT messages
  // Handle MQTT messages and retrieve displayed alphabet and user response (button pressed)
  // Modify the following code to handle MQTT messages and retrieve data accordingly
  // Example: String displayedAlphabet = getDisplayedAlphabetFromMQTT();
  //         String userResponse = getUserResponseFromMQTT();

  // Call the function to display the alphabet on the dot matrix display
  displayAlphabet(displayedAlphabet);

  // Record the displayed alphabet and user response in Firebase
  recordSessionData(displayedAlphabet, userResponse);

  // Additional code for your specific MQTT message handling logic
  // ...

  // Add any other required code for your application's functionality
  // ...
}

void displayAlphabet(char alphabet) {
  lc.clearDisplay(0);         // Clear the display
  lc.setChar(0, 0, alphabet, false);  // Display the alphabet on the display
}

void recordSessionData(char displayedAlphabet, char userResponse) {
  // Record session data in Firebase (displayed alphabet and user response)
  Firebase.pushString(fbdo, "sessions/displayed_alphabet", String(displayedAlphabet));
  Firebase.pushString(fbdo, "sessions/user_response", String(userResponse));
}
