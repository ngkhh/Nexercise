// still under development not ready to imply
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <FirebaseArduino.h>

// Dot Matrix Pins
#define MAX_DEVICES 4  // Number of MAX7219 modules
#define CLK_PIN D1     // CLK pin connected to the module's CLK pin
#define CS_PIN D2      // CS pin connected to the module's CS pin
#define DATA_PIN D3    // DIN pin connected to the module's DIN pin

// Button Pins
#define BUTTON_W D4
#define BUTTON_M D5

// WiFi
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "nexercise/project";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// Firebase
const char* firebase_host = "nexercise-69420.firebaseapp.com";
const char* firebase_auth = "AIzaSyAdoJOTRJejgaR3O6bQSw-xwW5OoLrs0i4";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Dot Matrix
MD_Parola dotMatrix = MD_Parola(MAX_DEVICES, DATA_PIN, CLK_PIN, CS_PIN, MD_MAX72XX::FC16_HW);

// Game Variables
const int maxRounds = 10;
int currentRound = 0;
bool gameActive = false;
bool restartRequested = false;
char currentLetter;

// Firebase Path
String firebasePath;

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setupDotMatrix() {
  dotMatrix.begin();
  dotMatrix.setInvert(false);
  dotMatrix.setIntensity(4);
  dotMatrix.displayClear();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages
  // Here, you can process the messages received from the controller
  // For example, if the message indicates a restart request, you can set the restartRequested flag
}

void connectToMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP8266Client")) {
      mqttClient.subscribe(mqtt_topic);
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("Failed to connect to MQTT Broker. MQTT Error code: ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void updateDotMatrix(const char* text) {
  dotMatrix.displayClear();
  dotMatrix.print(text, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

void startGame() {
  currentRound = 0;
  gameActive = true;
  restartRequested = false;
  
  // Publish the start of the game to MQTT topic
  mqttClient.publish(mqtt_topic, "start");

  // Update the dot matrix with the initial round
  updateDotMatrix("Round 1: W");
}

void endGame() {
  gameActive = false;
  restartRequested = false;

  // Publish the end of the game to MQTT topic
  mqttClient.publish(mqtt_topic, "end");

  // Clear the dot matrix display
  dotMatrix.displayClear();
}

void handleButtonPress(int buttonPin) {
  if (buttonPin == BUTTON_W && currentLetter == 'W') {
    // Correct button press for letter W
    currentRound++;
    if (currentRound < maxRounds) {
      char roundText[10];
      sprintf(roundText, "Round %d: W", currentRound + 1);
      updateDotMatrix(roundText);
    } else {
      // All rounds completed, end the game
      endGame();
    }
  } else if (buttonPin == BUTTON_M && currentLetter == 'M') {
    // Correct button press for letter M
    currentRound++;
    if (currentRound < maxRounds) {
      char roundText[10];
      sprintf(roundText, "Round %d: M", currentRound + 1);
      updateDotMatrix(roundText);
    } else {
      // All rounds completed, end the game
      endGame();
    }
  } else {
    // Incorrect button press, request restart
    restartRequested = true;
    updateDotMatrix("Wrong! Restarting...");
  }
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  setupDotMatrix();

  // Initialize MQTT and connect to the broker
  connectToMQTT();

  // Set up button inputs
  pinMode(BUTTON_W, INPUT_PULLUP);
  pinMode(BUTTON_M, INPUT_PULLUP);

  // Set the initial dot matrix display
  updateDotMatrix("Press Start");

  // Firebase Initialization
  Firebase.begin(firebase_host, firebase_auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Handle MQTT connection and incoming messages
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  // Check for button presses
  if (gameActive) {
    if (digitalRead(BUTTON_W) == LOW) {
      handleButtonPress(BUTTON_W);
      delay(200); // Debounce delay
    }
    if (digitalRead(BUTTON_M) == LOW) {
      handleButtonPress(BUTTON_M);
      delay(200); // Debounce delay
    }
  }

  // Check if restart is requested
  if (restartRequested) {
    startGame();
  }
}
