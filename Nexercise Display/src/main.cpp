#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase.h>
#include <ESP8266WiFi.h>
#include <LedControl.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#define THISDISPLAY 3 // display id (display number)

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// Insert your network credentials
#define WIFI_SSID "ni"
#define WIFI_PASSWORD "ck"

// Insert Firebase project API Key
#define API_KEY "1234 i declare a world war"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app"

// Pins for dot matrix displays
//const int dataPin = 14;
//const int clockPin = 12;
//const int csPin = 13;

// Sleep mode parameters
const unsigned long sleepTimeout = 3000000; // 50 minutes (50 min * 60 sec * 1000 ms)
unsigned long lastActivityTime = 0;

LedControl lc = LedControl(12, 15, 13, 1);

// User input pin for waking up from sleep mode
const int userInputPin = 4; // Example pin for user input

// Game variables
const int numDisplays = 4;
const int numRounds = 10;
char alphabet;
int currentDisplay;
int roundCount = 0;
int sessionNumber;
bool runningOnThisDisplay = false;
bool signupOK = false;
String userResponse;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int generateSessionNumber() {
  // Generate a random session number between 1 and 1000
  return random(1, 1001);
}


void displaySymbol(char s) {
  byte charCross[8] = {0x81, 0x42, 0x24, 0x08, 0x10, 0x24, 0x42, 0x81};
  byte charTick[8] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x90, 0xa0, 0x40};

  for (int i = 0; i < 8; i++) {
    //    lc.clearDisplay(0);
    lc.setRow(0, i, s == 'T' ? charTick[i] : charCross[i]);

  }
}

char randomAlphabet() {
  char alphabets[] = {'M', 'W'};
  int index = random(0, 2);
  return alphabets[index];
}

int randomDisplay() {
  return random(0, numDisplays);
}
void displayW() {
  byte W[8] = {0x82, 0x82, 0x92, 0x92, 0x92, 0xaa, 0xc6, 0x82};
  lc.setRow(0, 0, W[0]);
  lc.setRow(0, 1, W[1]);
  lc.setRow(0, 2, W[2]);
  lc.setRow(0, 3, W[3]);
  lc.setRow(0, 4, W[4]);
  lc.setRow(0, 5, W[5]);
  lc.setRow(0, 6, W[6]);
  lc.setRow(0, 7, W[7]);
  delay(10);
}

void displayM() {
  byte M[8] = {0x82, 0xc6, 0xaa, 0x92, 0x92, 0x82, 0x82, 0x82};
  lc.setRow(0, 0, M[0]);
  lc.setRow(0, 1, M[1]);
  lc.setRow(0, 2, M[2]);
  lc.setRow(0, 3, M[3]);
  lc.setRow(0, 4, M[4]);
  lc.setRow(0, 5, M[5]);
  lc.setRow(0, 6, M[6]);
  lc.setRow(0, 7, M[7]);
  delay(10);
}


void displayAlphabet(char alphabet, int displayIndex) {
  //lc.clearDisplay(0); // Clear the display

  if (alphabet == 'M') {
    //    for (int i = 0; i < 8; i++) {
    //      lc.setRow(0, i, charM[i]);  // Display 'M' pattern on the selected display
    //    }
    displayM();
  } else if (alphabet == 'W') {
    //    for (int i = 0; i < 8; i++) {
    //      lc.setRow(0, i, charW[i]);  // Display 'W' pattern on the selected display
    //    }
    displayW();
  }

  //lc.setChar(0, displayIndex, ' ', false); // Display the alphabet (no selected displays)
}


void updateFirebaseActivityTime() {
  // Update last activity time in Firebase when user interacts with controller
  String path = "";
  unsigned long lastActivityTime = timeClient.getEpochTime();
  Serial.println(lastActivityTime);
  delay(500);
  if (Firebase.RTDB.setString(&fbdo, (path + "/lastActivityTime"), String(lastActivityTime))) {
    Serial.println("firebaseactivitytime Success");
  }
  else {
    Serial.println("firebaseactivitytime Failed");
  }
}
void playThisRound() {
  if (alphabet != 'M' && alphabet != 'W') {
    Serial.println("bad alphabet to show!");
    return;
  }
  if (currentDisplay == THISDISPLAY) {
    displayAlphabet(alphabet, currentDisplay);
  }
}

void sendToDisplay(char alphabet, int displayIndex) {
  //save to firebase the alphabet to display in the next random display
  String path = "/current/";
  FirebaseJson json;
  json.set(("alphabet"), String(alphabet));
  json.set(("displayIndex"), displayIndex);
  json.set(("roundCount"), roundCount);
  json.set(("sessionNumber"), sessionNumber);
  Firebase.RTDB.setJSON(&fbdo, (path), &json);
}

void playNextRound() {
  roundCount++;
  alphabet = randomAlphabet();
  currentDisplay = randomDisplay();
  //currentDisplay = 1;
  Serial.print("Round ");
  Serial.print(roundCount);
  Serial.print(" - Display: ");
  Serial.print(currentDisplay);
  Serial.print(" | Alphabet: ");
  Serial.println(alphabet);
  if (currentDisplay == THISDISPLAY) {
    sendToDisplay(alphabet, currentDisplay);
  }
  else {
    sendToDisplay(alphabet, currentDisplay);
  }


}

void recordSessionStart() {
  // Record session start time in Firebase
  unsigned long sessionStartTime = timeClient.getEpochTime();
  String path = "sessions/" + String(sessionNumber);
  if (Firebase.RTDB.setString(&fbdo, (path + "/start_time"), String(sessionStartTime))) {
    Serial.print("Start time: ");
    Serial.print(sessionStartTime);
  }
}

void startNewSession() {
  roundCount = 0;
  sessionNumber = generateSessionNumber();
  Serial.print("Starting session number: ");
  Serial.println(sessionNumber);

  recordSessionStart();
  playNextRound();
}

void continueSession() {
  Serial.print("Continuing session number: ");
  Serial.println(sessionNumber);

  playThisRound();
}


void resetRound() {
  roundCount = 0;
  alphabet = randomAlphabet();
  currentDisplay = randomDisplay();
  //currentDisplay = 1;
  Serial.print("Round ");
  Serial.print(roundCount);
  Serial.print(" - Display: ");
  Serial.print(currentDisplay);
  Serial.print(" | Alphabet: ");
  Serial.println(alphabet);
  if (currentDisplay == THISDISPLAY) {
    sendToDisplay(alphabet, currentDisplay);
  }
  else {
    sendToDisplay(alphabet, currentDisplay);
  }
}


void recordSessionEnd() {
  // Record session end time in Firebase
  unsigned long sessionEndTime = timeClient.getEpochTime();
  String path = "sessions/" + String(sessionNumber);
  if (Firebase.RTDB.setString(&fbdo, (path + "/end_time"), String(sessionEndTime))) {
    Serial.print("End time: ");
    Serial.print(sessionEndTime);
  }

  //not needed, because startnewsession() which is called after this
  //Firebase.setString(fbdo, (path + "current/roundCount"), String(0)); //roundCount global variable
  //Firebase.setString(fbdo, (path + "current/sessionNumber"), String(0)); //sessionNumber global variable
}
void validateResponse(String message) {
  // start with this
  // Serial.println("Validation");
  // Serial.println(message);
  // Serial.println(alphabet);
  if (message[0] == alphabet) {
    //show tick if correct
    char charToDisplay = 'T';
    displaySymbol(charToDisplay);
    Firebase.RTDB.setString(&fbdo, "controller1/response", "0");
    // move to next round!
    playNextRound();
  }
  else {
    // show cross if selected wrong alphabet
    char charToDisplay = 'X';
    displaySymbol(charToDisplay);
    Firebase.RTDB.setString(&fbdo, "controller1/response", "0");
    // also reset round)
    resetRound();
  }
}




void setup() {
  Serial.begin(115200);

  timeClient.begin();
  timeClient.setTimeOffset(28800);
  timeClient.setUpdateInterval(1000);

  // Initialize dot matrix displays
  lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear displays

  // Initialize user input pin
  pinMode(userInputPin, INPUT_PULLUP);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
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
  if (runningOnThisDisplay == false) {
    // check if THIS display is chosen
    if (Firebase.ready()) {
      if (Firebase.RTDB.getJSON(&fbdo, "/current/")) {
        FirebaseJson &json = fbdo.to<FirebaseJson>();
        FirebaseJsonData result;
        json.get(result, "alphabet");
        if (result.success) {
          alphabet = (result.to<String>)()[0];
        }
        json.get(result, "displayIndex");
        if (result.success) {
          currentDisplay = result.to<int>();
        }
        json.get(result, "roundCount");
        if (result.success) {
          roundCount = result.to<int>();
        }
        json.get(result, "sessionNumber");
        if (result.success) {
          sessionNumber = result.to<int>();
        }
      }
      //Serial.println("alphabet");
      // Serial.println("currentDisplay");
      // Serial.println("roundCount");
      Serial.println("sessionNumber");
      // Serial.println(alphabet);
      // Serial.println(currentDisplay);
      // Serial.println(roundCount);
      Serial.println(sessionNumber);
      Serial.println("Display number");
      Serial.println(THISDISPLAY);

      if (currentDisplay == THISDISPLAY) {
        runningOnThisDisplay = true;
        if (roundCount >= 1) {
          continueSession();
        }
        else if (roundCount == 0) {
          startNewSession();
        }
      }
    }
  }
  else if (runningOnThisDisplay == true) {
    // check for button press from controller
    // updateFirebaseActivityTime(); // Update last activity time only when active
    if (Firebase.ready()) {
      if (Firebase.RTDB.getString(&fbdo, "controller1/response")) {
        userResponse = fbdo.to<String>();
        delay(1000);
      }
      //      Firebase.RTDB.setString(&fbdo, "controller1/response", "0");
      if (userResponse != "0") {
        validateResponse(userResponse); // after validating, run
        runningOnThisDisplay = false;
        delay(1000);
        lc.clearDisplay(0);
      }
    }
  }

  // Check for user inactivity to enter sleep mode
  /*if (millis() - lastActivityTime >= sleepTimeout) {
    enterSleepMode();
    }*/
  // timeClient.update();
  // Serial.print("Received Time: ");
  // Serial.println(timeClient.getFormattedTime());

  // Check for session completion
  // if (roundCount >= numRounds) {
  //   // All rounds completed for the session
  //   Serial.println("Session completed! Starting a new session");
  //   recordSessionEnd();
  //   startNewSession();
  // }
}


