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

void ICACHE_RAM_ATTR handleButton1() {
  Serial.println("Button 1 Pressed");
  // Toggle the button 1 status
  button1Status = !button1Status;

  // Send the updated button 1 status to the client-side ESP8266
  String status = button1Status ? "on" : "off";
  server.send(200, "text/plain", status);
}

void ICACHE_RAM_ATTR handleButton2() {
  Serial.println("Button 2 Pressed");
  // Toggle the button 2 status
  button2Status = !button2Status;

  // Send the updated button 2 status to the client-side ESP8266
  String status = button2Status ? "on" : "off";
  server.send(200, "text/plain", status);
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Server-side ESP8266</h1>";
  html += "<h2>Button 1 Status: ";
  html += button1Status ? "ON" : "OFF";
  html += "</h2>";
  html += "<h2>Button 2 Status: ";
  html += button2Status ? "ON" : "OFF";
  html += "</h2>";
  html += "<h3><a href=\"/button1\">Toggle Button 1</a></h3>";
  html += "<h3><a href=\"/button2\">Toggle Button 2</a></h3>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  // Start Serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi. IP:");
  Serial.print(WiFi.localIP());

  // Configure the button pins as input with internal pull-up resistors
  pinMode(5, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  // Attach interrupts to the button pins to detect button presses
  attachInterrupt(digitalPinToInterrupt(5), handleButton1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(14), handleButton2, CHANGE);

  // Start the server
  server.on("/", handleRoot);
  server.on("/button1", handleButton1);
  server.on("/button2", handleButton2);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
