#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Whateverthisis"; // Your WiFi SSID
const char* password = "VeryGoodPassword"; // Your WiFi password
const String serverURL = "http://192.168.116.207"; // Replace with the IP address of your server-side ESP8266

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
  // Control LED based on button status received from server-side
  // Example: send HTTP requests to server-side and update LED based on response
  
  // Example: send request for Button 1 status
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
  
  // Example: send request for Button 2 status
  // Similar to Button 1, update LED based on response
  
  delay(1000);
}
