#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

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

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Dot Matrix
MD_Parola dotMatrix = MD_Parola(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Game Variables
const int maxRounds = 10;
int currentRound = 0;
bool gameActive = false;
bool restartRequested = false;
char currentLetter;

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
      mqttClient.subscribe(topic);
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
  mqttClient.publish(topic, "start");

  // Update the dot matrix with the initial round
  updateDotMatrix("Round 1: W");
}

void endGame() {
  gameActive = false;
  restartRequested = false;

  // Publish the end of the game to MQTT topic
  mqttClient.publish(topic, "end");
  
// Update the dot matrix with the end message
  updateDotMatrix("Game Over!");

  // Delay for a few seconds before clearing the dot matrix
  delay(5000);
  dotMatrix.displayClear();
}

void restartGame() {
  currentRound = 0;
  gameActive = true;
  restartRequested = false;

  // Publish the restart of the game to MQTT topic
  mqttClient.publish(topic, "restart");

  // Update the dot matrix with the initial round
  updateDotMatrix("Round 1: W");
}

void handleButtonPress(int buttonPin) {
  if (buttonPin == BUTTON_W && gameActive && !restartRequested) {
    // Process button press for the 'W' button
    // Here, you can implement the logic for handling the button press for the 'W' button
    // For example, you can check if the pressed letter matches the expected letter
    // and update the round and dot matrix accordingly
    // You can also handle the end of the game or request for restart if needed
    if (currentLetter == 'W') {
      currentRound++;
      if (currentRound <= maxRounds) {
        // Generate the next letter and update the dot matrix
        currentLetter = generateRandomLetter();
        char roundText[10];
        sprintf(roundText, "Round %d: %c", currentRound, currentLetter);
        updateDotMatrix(roundText);
      } else {
        // End the game if the maximum number of rounds is reached
        endGame();
      }
    } else {
      // Handle incorrect button press for the 'W' button
      // For example, display an error message on the dot matrix
      updateDotMatrix("Wrong button!");
      // You can also implement additional logic, such as deducting points or ending the game
    }
  } else if (buttonPin == BUTTON_M && gameActive) {
    // Process button press for the 'M' button
    // Here, you can implement the logic for handling the button press for the 'M' button
    // For example, you can handle the request for restart or end the game if needed
    if (restartRequested) {
      // Restart the game if restart is requested
      restartGame();
    } else {
      // End the game if the 'M' button is pressed
      endGame();
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup WiFi connection
  setupWiFi();

  // Setup dot matrix display
  setupDotMatrix();

  // Connect to MQTT broker
  connectToMQTT();

  // Setup button pins
  pinMode(BUTTON_W, INPUT_PULLUP);
  pinMode(BUTTON_M, INPUT_PULLUP);

  // Attach interrupts for button pins
  attachInterrupt(digitalPinToInterrupt(BUTTON_W), []() {
    handleButtonPress(BUTTON_W);
  }, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_M), []() {
    handleButtonPress(BUTTON_M);
  }, FALLING);
}

void loop() {
  // Reconnect to MQTT if connection lost
  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  // Process MQTT messages
  mqttClient.loop();

  // Handle restart request
  if (restartRequested) {
    restartGame();
  }

  // Handle game end
  if (currentRound >= maxRounds) {
    endGame();
  }
}
