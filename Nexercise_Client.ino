#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Whateverthisis"; // Your WiFi SSID
const char* password = "VeryGoodPassword"; // Your WiFi password
const String serverURL = "http://192.168.219.207"; // Replace with the IP address of your server-side ESP8266

const int ledPin = 13; // Pin for LED

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Send HTTP requests to server-side and update LED based on response
  
  // Send request for Button 1 status
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
  
  // Send request for Button 2 status
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
