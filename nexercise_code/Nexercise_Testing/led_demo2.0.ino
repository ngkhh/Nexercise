#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// GPIO 5 D1
#define LED 5
#define onButton 13
#define offButton 0

int previousOffButtonState; 
int presentOffButtonState;

int previousOnButtonState; 
int presentOnButtonState;

// WiFi
const char *ssid = "your_wifi_ssid"; // Enter your WiFi name
const char *password = "your_wifi_password";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp8266/led";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

bool ledState = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    delay(1000); // Delay for stability

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    //Setting button pin as input
    pinMode(offButton, INPUT_PULLUP);
    pinMode(onButton, INPUT_PULLUP);
    presentOffButtonState = digitalRead(offButton);

    // Setting LED pin as output
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);  // Turn off the LED initially

    // Connecting to an MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Publish and subscribe
    client.publish(topic, "hello emqx");
    client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char) payload[i];  // Convert *byte to string
    }
    Serial.print(message);
    if (message == "on" && !ledState) {
        digitalWrite(LED, HIGH);  // Turn on the LED
        ledState = true;
    }
    if (message == "off" && ledState) {
        digitalWrite(LED, LOW); // Turn off the LED
        ledState = false;
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
   previousOffButtonState = presentOffButtonState;
   presentOffButtonState = digitalRead(offButton);
   if(previousOffButtonState == HIGH && presentOffButtonState == LOW) {
    Serial.println("Off button is pressed");
    client.publish("esp8266/led", "off");
   }

   previousOnButtonState = presentOnButtonState;
   presentOnButtonState = digitalRead(onButton);
   if(previousOnButtonState == HIGH && presentOnButtonState == LOW) {
    Serial.println("On button is pressed");
    client.publish("esp8266/led", "on");
   }
   
    client.loop();
    delay(100); // Delay for a short period in each loop iteration
}
