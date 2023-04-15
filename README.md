# Nexercise

## Currently facing these problems:
- Getting fucked up by ESP shit 
- The message can be only display once

## Currently working on
- Dot matrix showing randomized alphabet
- 2 buttons represent 'W' and 'M' 

## Source of ESP Library 
- https://github.com/esp8266/Arduino

## ESP board 
- https://arduino.esp8266.com/stable/package_esp8266com_index.json

## How to use an ESP board
- https://electropeak.com/learn/how-to-use-esp8266-01-pins-and-leds/

## Dot Matrix docs
- https://www.open-electronics.org/led-dot-matrix-display-with-nodemcu/


# MQTT
## Setting up a local MQTT broker using Mosquitto:

- Install Mosquitto broker software on your computer or a dedicated device like a Raspberry Pi. You can download the software from the Mosquitto website.

- After installation, open the Mosquitto configuration file, which is usually located in the installation directory. The file is named "mosquitto.conf". You can edit the file using a text editor like Notepad or Sublime Text.

- In the configuration file, you can specify the port number for the broker to listen on, enable security features like TLS/SSL encryption, and set up authentication for clients to connect to the broker. You can find detailed instructions on how to configure Mosquitto in the Mosquitto documentation.

- After configuring the Mosquitto broker, save the configuration file and start the broker by running the command "mosquitto -c /path/to/mosquitto.conf" on the terminal or command prompt.

- Verify that the Mosquitto broker is running by subscribing to a topic using the Mosquitto command-line tool or a GUI tool like MQTT.fx. You should be able to see the messages published to the topic.

## Mosquitto documentation: 
- https://mosquitto.org/documentation/

## MQTT client library for ESP8266: 
- https://github.com/knolleary/pubsubclient

## Getting started with MQTT and ESP8266: 
- https://randomnerdtutorials.com/what-is-mqtt-and-how-it-works/

## Connecting ESP8266 to MQTT broker with TLS/SSL encryption: 
- https://diyprojects.io/connect-esp8266-mqtt-broker-tls-ssl-lets-encrypt/#.YHwv8-gzY2w
