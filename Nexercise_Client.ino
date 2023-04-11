#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Whateverthisis"; // Your WiFi SSID
const char* password = "VeryGoodPassword"; // Your WiFi password
const char* serverAddress = "192.168.82.236"; // IP address of the server
int serverPort = 80; // Port number of the server
String url = "/data/?sensor0_reading=0&sensor1_reading=0"; // URL of the server
int button1Pin = 5; // Pin number for button 1
int button2Pin = 14; // Pin number for button 2
bool button1State = false; // State of button 1
bool button2State = false; // State of button 2
WiFiClient wifiClient; // WiFi client object

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

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
}

void loop() {
  // Read button states
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Send GET request to turn on the LED on the server-side
    if (button1State == LOW) {
      http.begin(wifiClient, serverAddress, serverPort, "/led/on");
      int httpResponseCode = http.GET();
      if (httpResponseCode == 200) {
        Serial.println("LED turned on");
      } else {
        Serial.print("Error turning on LED. HTTP response code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }

    // Send GET request to turn off the LED on the server-side
    if (button2State == LOW) {
      http.begin(wifiClient, serverAddress, serverPort, "/led/off");
      int httpResponseCode = http.GET();
      if (httpResponseCode == 200) {
        Serial.println("LED turned off");
      } else {
        Serial.print("Error turning off LED. HTTP response code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }

    // Send GET request to receive button status
    http.begin(wifiClient, serverAddress, serverPort, url);
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Button status: " + response);
    } else {
      Serial.print("Error receiving button status. HTTP response code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  delay(100); // Add a small delay to debounce the buttons
}
