#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "Whateverthisis"; // Your WiFi SSID
const char* password = "VeryGoodPassword"; // Your WiFi password

// Create an instance of the server
ESP8266WebServer server(80);

// Variables to store the button status
volatile bool button1Status = false;
volatile bool button2Status = false;

// Variables to store the last button state and the last time the button was debounced
volatile bool lastButton1State = HIGH;
volatile bool lastButton2State = HIGH;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay = 50; // Debounce delay in milliseconds

// Function prototype for handleRoot
void handleRoot();

void ICACHE_RAM_ATTR handleButton1() {
  // Debounce the button input
  if ((millis() - lastDebounceTime1) < debounceDelay) {
    return;
  }
  
  lastDebounceTime1 = millis();

  // Read the current state of the button
  bool button1State = digitalRead(5);

  // Check if the button state has changed
  if (button1State != lastButton1State) {
    // Update the last button state
    lastButton1State = button1State;

    // Check if the button is pressed
    if (button1State == LOW) {
      Serial.println("Button 1 Pressed");
      // Toggle the button 1 status
      button1Status = !button1Status;

      // Send the updated button 1 status to the client-side ESP8266
      String status = button1Status ? "on" : "off";
      server.send(200, "text/plain", status);
    }
  }
}

void ICACHE_RAM_ATTR handleButton2() {
  // Debounce the button input
  if ((millis() - lastDebounceTime2) < debounceDelay) {
    return;
  }
  
  lastDebounceTime2 = millis();

  // Read the current state of the button
  bool button2State = digitalRead(14);

  // Check if the button state has changed
  if (button2State != lastButton2State) {
    // Update the last button state
    lastButton2State = button2State;

    // Check if the button is pressed
    if (button2State == LOW) {
      Serial.println("Button 2 Pressed");
      // Toggle the button 2 status
      button2Status = !button2Status;

      // Send the updated button 2 status to the client-side ESP8266
      String status = button2Status ? "on" : "off";
      server.send(200, "text/plain", status);
    }
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up the buttons as inputs
  pinMode(5, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  // Attach interrupts to the buttons
  attachInterrupt(digitalPinToInterrupt(5), handleButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(14), handleButton2, FALLING);

// Start the server
server.begin();
Serial.println("Server started");

// Print the ESP8266 IP address
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
}

void loop() {
// Handle client requests
server.handleClient();
}

void handleRoot() {
// Serve the HTML page with the button status
String html = "<html><body>";
html += "<h1>Button 1: " + String(button1Status ? "ON" : "OFF") + "</h1>";
html += "<h1>Button 2: " + String(button2Status ? "ON" : "OFF") + "</h1>";
html += "</body></html>";
server.send(200, "text/html", html);
}
