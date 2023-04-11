#include <ESP8266WiFi.h>

#define ledPin 13 // Pin connected to the LED
#define button1Pin 5 // Pin connected to button 1
#define button2Pin 14 // Pin connected to button 2

const char* ssid = "Whateverthisis"; // Your WiFi SSID
const char* password = "VeryGoodPassword"; // Your WiFi password

WiFiServer server(80); // Create a WiFi server on port 80

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT); // Set the LED pin as OUTPUT
  pinMode(button1Pin, INPUT_PULLUP); // Set button 1 pin as INPUT_PULLUP
  pinMode(button2Pin, INPUT_PULLUP); // Set button 2 pin as INPUT_PULLUP

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin(); // Start the server
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available(); // Check for client connection

  if (client) {
    Serial.println("New client connected");

    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r'); // Read client request
        client.flush();

        if (request.indexOf("/led/on") != -1) { // If request contains /led/on
          digitalWrite(ledPin, HIGH); // Turn on the LED
          client.println("LED turned on");
        }
        else if (request.indexOf("/led/off") != -1) { // If request contains /led/off
          digitalWrite(ledPin, LOW); // Turn off the LED
          client.println("LED turned off");
        }
        else {
          client.println("Invalid request"); // Send error message for invalid requests
        }

        // Check button 1 status
        if (digitalRead(button1Pin) == LOW) {
          client.println("Button 1 pressed"); // Send message to client about button 1 status
        }

        // Check button 2 status
        if (digitalRead(button2Pin) == LOW) {
          client.println("Button 2 pressed"); // Send message to client about button 2 status
        }

        delay(10);
        client.stop(); // Close the connection
        Serial.println("Client disconnected");
      }
    }
  }
}
