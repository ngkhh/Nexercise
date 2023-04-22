# Nexercise

## for NIGEE
https://cedalo.com/blog/mqtt-connection-beginners-guide/

## Finally something useful
- https://console.hivemq.cloud/clients/arduino-esp8266?uuid=597c9bc0b8d0400b8f723ca742f37e73
### tis might help
- https://community.hivemq.com/t/arduino-example-with-server-signed-certificate/502

## MQTT help suggested by Wong Yi Jin (Senior Alumni)
- https://techtutorialsx.com/2017/04/24/esp32-publishing-messages-to-mqtt-topic/
- https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/

## Currently facing these problems:
- Laziness
- Exam being very soon
- Lack of motivation
- Skill issue
- MQTT is not connecting with error code                   rc=-4

## Currently working on
- MQTT on ESP8266(s)
- Dot matrix showing randomized alphabet
- 2 buttons represent "W" and "M" 

## Planned
- Connecting 4 ESP8266 with MQTT
- Combine the buttons and dot matrix to create a game

### Source of ESP Library 
- https://github.com/esp8266/Arduino

### ESP board 
- https://arduino.esp8266.com/stable/package_esp8266com_index.json

### How to use an ESP board
- https://electropeak.com/learn/how-to-use-esp8266-01-pins-and-leds/

### Dot Matrix docs
- https://www.open-electronics.org/led-dot-matrix-display-with-nodemcu/


# MQTT
## Setting up a local MQTT broker using Mosquitto:

1. Install Mosquitto broker software on your computer or a dedicated device like a Raspberry Pi. You can download the software from the Mosquitto website.

2. After installation, open the Mosquitto configuration file, which is usually located in the installation directory. The file is named `"mosquitto.conf"`. You can edit the file using a text editor like Notepad or Sublime Text.

3. In the configuration file, you can specify the port number for the broker to listen on, enable security features like TLS/SSL encryption, and set up authentication for clients to connect to the broker. You can find detailed instructions on how to configure Mosquitto in the Mosquitto documentation.

4. After configuring the Mosquitto broker, save the configuration file and start the broker by running the command `"mosquitto -c /path/to/mosquitto.conf"` on the terminal or command prompt.

5. Verify that the Mosquitto broker is running by subscribing to a topic using the Mosquitto command-line tool or a GUI tool like MQTT.fx. You should be able to see the messages published to the topic.

## Connecting your ESP8266 devices to the MQTT broker:

1. Install the MQTT client library for ESP8266 on your development environment. You can find the library on the Arduino IDE or PlatformIO library manager.

2. Import the MQTT library to your ESP8266 project, and initialize the MQTT client object by providing the MQTT broker's IP address or hostname, port number, and authentication credentials.

3. In your code, you can create MQTT topics that your ESP8266 devices can publish and subscribe to, such as sensor readings or control commands.

4. Use the MQTT client library to publish messages to the broker or subscribe to messages on specific topics. You can find examples of how to use the MQTT client library in the library's documentation or online tutorials.

### MQTT 
MQTT Essentials by HiveMQ: https://www.hivemq.com/mqtt-essentials/

MQTT.fx - a graphical MQTT client: https://mqttfx.jensd.de/

Eclipse Paho - a set of MQTT client libraries: https://www.eclipse.org/paho/

Adafruit IO - a cloud-based MQTT service: https://io.adafruit.com/

CloudMQTT - a cloud-based MQTT service: https://www.cloudmqtt.com/

### Mosquitto documentation: 
- https://mosquitto.org/documentation/

### MQTT client library for ESP8266: 
- https://github.com/knolleary/pubsubclient

### Getting started with MQTT and ESP8266: 
- https://randomnerdtutorials.com/what-is-mqtt-and-how-it-works/

### Connecting ESP8266 to MQTT broker with TLS/SSL encryption: 
- https://diyprojects.io/connect-esp8266-mqtt-broker-tls-ssl-lets-encrypt/#.YHwv8-gzY2w
