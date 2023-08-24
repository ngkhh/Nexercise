#include <Firebase.h>
#include <ESP8266WiFi.h>
#include <LedControl.h>
  
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#define THISDISPLAY 1 // display id (display number)

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// Insert your network credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "nexercise-69420-default-rtdb.asia-southeast1.firebasedatabase.app" 

// Pins for dot matrix displays
const int dataPin = 14;
const int clockPin = 12;
const int csPin = 13;

// Sleep mode parameters
const unsigned long sleepTimeout = 3000000; // 50 minutes (50 min * 60 sec * 1000 ms)
unsigned long lastActivityTime = 0;

LedControl lc = LedControl(dataPin, clockPin, csPin, 1); // Initialize LedControl object

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

void setup() {
  Serial.begin(115200);

  // Initialize dot matrix displays
  lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear displays

  // Initialize user input pin
  pinMode(userInputPin, INPUT_PULLUP);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
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
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
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
      
      // Current alphabet
      if(Firebase.RTDB.getString(&fbdo, "/current/alphabet")){
        alphabet = fbdo.to<String>()[0];
      }
      
      // Current display index
      if(Firebase.RTDB.getString(&fbdo, "/current/displayIndex")){
        currentDisplay = fbdo.to<int>();
      }

      // Current round count
      if(Firebase.RTDB.getString(&fbdo, "/current/roundCount")){
        roundCount = fbdo.to<int>();
      }
      
      // sessionNumber
      if(Firebase.RTDB.getString(&fbdo, "/current/sessionNumber")){
        sessionNumber = fbdo.to<int>();
      }
      
      lastActivityTime = millis(); // Update activity time
      //updateFirebaseActivityTime(); // Update last activity time in Firebase

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
    if (Firebase.ready()) {
      if (Firebase.RTDB.getString(&fbdo, "/controller1/response")){
        userResponse = fbdo.to<String>()[0];
      }
      Firebase.RTDB.setString(&fbdo, "/controller1/response", "0");
      if (userResponse != "0"){
        validateResponse(userResponse); // after validating, run
        runningOnThisDisplay = false;
      }
    }
  }
  // Check for user inactivity to enter sleep mode
  /*if (millis() - lastActivityTime >= sleepTimeout) {
    enterSleepMode();
  }*/

  // Check for session completion
  if (roundCount >= numRounds) {
    // All rounds completed for the session
    Serial.println("Session completed! Starting a new session");
    recordSessionEnd();
    startNewSession();
  }

  delay(100);
}

void validateResponse(String message) {
  // start with this
  if (message.charAt(0) == alphabet) {
    //show tick if correct
    char charToDisplay = 'T';
    displaySymbol(charToDisplay);
    lastActivityTime = millis(); // Update activity time
    //updateFirebaseActivityTime(); // Update last activity time in Firebase
    // move to next round! 
    playNextRound();
  }
  else {
    // show cross if selected wrong alphabet
    char charToDisplay = 'X';
    displaySymbol(charToDisplay);
    lastActivityTime = millis(); // Update activity time
    //updateFirebaseActivityTime(); // Update last activity time in Firebase
    // also move to next round (idk)
    playNextRound();
  }
}

void displaySymbol(char c) {
  byte charTick[8] = {B00000000, B00000010,B00000101,B00001000,B00010000,B10100000,B01000000,B00000000};
  byte charCross[8] = {  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B10000001};
  
  for (int i = 0; i < 8; i++) {
    lc.clearDisplay(0);
    lc.setRow(0, i, c == 'T' ? charTick[i] : charCross[i]);
  }
}

/*void updateFirebaseActivityTime() {
  // Update last activity time in Firebase when user interacts with controller
  //String path = "/display/" + String(THISDISPLAY);
  Firebase.RTDB.setString(&fbdo, ("lastActivityTime/"), String(lastActivityTime));
}*/

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

void playThisRound() {
  if (currentDisplay == THISDISPLAY) {
    displayAlphabet(alphabet, currentDisplay);
  }
}

void playNextRound() {

  roundCount++;
  alphabet = randomAlphabet();
  currentDisplay = randomDisplay();
  if (currentDisplay == THISDISPLAY) {
    displayAlphabet(alphabet, currentDisplay);
  }
  else {
    sendToDisplay(alphabet, currentDisplay);
  }

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
  lc.setChar(0, displayIndex, alphabet, false); // Display the alphabet (no selected displays)
}

void sendToDisplay(char alphabet, int displayIndex) {
  //save to firebase the alphabet to display in the next random display
  String path = "/";
  Firebase.RTDB.setString(&fbdo, (path + "current/alphabet"), String(alphabet));
  Firebase.RTDB.setString(&fbdo, (path + "current/displayIndex"), String(displayIndex));
  Firebase.RTDB.setString(&fbdo, (path + "current/roundCount"), String(roundCount)); //roundCount global variable
  Firebase.RTDB.setString(&fbdo, (path + "current/sessionNumber"), String(sessionNumber)); //roundCount global variable
}

void recordSessionStart() {
  // Record session start time in Firebase
  unsigned long sessionStartTime = millis();
  String path = "/sessions/" + String(sessionNumber);
  Firebase.RTDB.setString(&fbdo, (path + "/start_time"), String(sessionStartTime));
}

void recordSessionEnd() {
  // Record session end time in Firebase
  unsigned long sessionEndTime = millis();
  String path = "/sessions/" + String(sessionNumber);
  Firebase.RTDB.setString(&fbdo, (path + "/end_time"), String(sessionEndTime));

//not needed, because startnewsession() which is called after this
  //Firebase.setString(fbdo, (path + "current/roundCount"), String(0)); //roundCount global variable
  //Firebase.setString(fbdo, (path + "current/sessionNumber"), String(0)); //sessionNumber global variable
}

int generateSessionNumber() {
  // Generate a random session number between 1 and 1000
  return random(1, 1001);
}
