#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase.h>
#include <ESP8266WiFi.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Pins for the buttons and buzzer
const int buttonM = 4;
const int buttonW = 5;
const int buzzer = 15;

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// Insert your network credentials
#define WIFI_SSID "CLPHS_Admin"
#define WIFI_PASSWORD "smclp8283655" 
/* they fucking changed admin password so this doesnt work anymore */

// Insert Firebase project API Key
#define API_KEY "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app"

// Bad user record
const unsigned long badUserRecord = 2400000; // 40 minutes (40 mins * 60 secs * 1000ms)

// Variables
bool signupOK = false;

int previousMButtonState;
int presentMButtonState;

int previousWButtonState;
int presentWButtonState;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  Serial.begin(115200);

  timeClient.begin();
  timeClient.setTimeOffset(28800);

  // Initialize buttons and buzzer
  pinMode(buttonM, INPUT_PULLUP);
  pinMode(buttonW, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase OK");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message);
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Get current time from NTP server
  timeClient.update();

  // Read button states
  int buttonWState = digitalRead(buttonW);

  // Perform actions based on button states
  previousMButtonState = presentMButtonState;
  int buttonMState = digitalRead(buttonM);
  if (previousMButtonState == HIGH && presentMButtonState == LOW) {
    Serial.println("Button M Pressed");
    String path = "/controller1/";
    Firebase.RTDB.setString(&fbdo, (path + "/response"), String("M"));
  }

  if (buttonWState == HIGH) {
  }

  // Your Firebase-related code can go here
  // For example, updating the database, reading data, etc.

  // Your other loop code can continue here
}
