#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "Ohio Wifi"; // Your WiFi SSID
const char* password = "69696969"; // Your WiFi password
const String serverURL = "http://192.168.102.207"; // Replace with the IP address of your server-side ESP8266

const int ledPin = 13; // Pin for LED
const int button1Pin = 5; // Pin for Button 1 (D1)
const int button2Pin = 14; // Pin for Button 2 (D5)

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read state of Button 1
  bool button1State = digitalRead(button1Pin);
  if (button1State == LOW) {
    Serial.println("Button 1 is pressed");
    digitalWrite(ledPin, HIGH); // Turn on LED
  } else {
    digitalWrite(ledPin, LOW); // Turn off LED
  }

  // Read state of Button 2
  bool button2State = digitalRead(button2Pin);
  if (button2State == LOW) {
    Serial.println("Button 2 is pressed");
    // Do something else here if desired
  }

  // Send HTTP requests to server-side and update LED based on response
  WiFiClient client; // Create a WiFiClient object
  HTTPClient http;
  http.begin(client, serverURL + "/button1"); // Pass the WiFiClient object to HTTPClient.begin()
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Button 1 is pressed");
      digitalWrite(ledPin, HIGH); // Turn on LED
    } else {
      Serial.println("Button 1 is not pressed");
      digitalWrite(ledPin, LOW); // Turn off LED
    }
  } else {
    Serial.println("Error sending request");
  }
  http.end();

  http.begin(client, serverURL + "/button2"); // Update URL to Button 2
  httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Button 2 is pressed");
      // Do something else here if desired
    } else {
      Serial.println("Button 2 is not pressed");
      // Do something else here if desired
    }
  } else {
    Serial.println("Error sending request");
  }
  http.end();

  delay(1000); // Wait for a second before sending the next request
}
