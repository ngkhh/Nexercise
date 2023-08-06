#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

FirebaseData fbdo;

// Constants for Wi-Fi connection
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Firebase configuration
const char* firebaseHost = "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* firebaseAuth = "your_firebase_auth_token";

// Pins for buttons
const int buttonWPIN = D1;
const int buttonMPIN = D2;

// Firebase variables
int sessionNumber;
unsigned long sessionStartTime;
char pressedButton;

void setupFirebase() {
  Firebase.begin(firebaseHost, firebaseAuth);
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonWPIN, INPUT_PULLUP);
  pinMode(buttonMPIN, INPUT_PULLUP);

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
  sessionNumber = generateSessionNumber();
  sessionStartTime = millis();
  Serial.print("Starting session number: ");
  Serial.println(sessionNumber);
}

void loop() {
  checkUserInput();
}

void checkUserInput() {
  if (digitalRead(buttonWPIN) == LOW) {
    handleUserInput('W');
  } else if (digitalRead(buttonMPIN) == LOW) {
    handleUserInput('M');
  }
}

void handleUserInput(char userInput) {
  pressedButton = userInput;
  recordUserButton(pressedButton, sessionNumber);
}

int generateSessionNumber() {
  // Generate a random session number between 1 and 1000
  return random(1, 1001);
}

void recordUserButton(char pressedButton, int sessionNumber) {
  String path = "sessions/" + String(sessionNumber);
  Firebase.setString(fbdo, (path + "/pressed_button").c_str(), String(pressedButton).c_str());
}
