FQBN ?= esp8266:esp8266:nodemcuv2
PORT ?= /dev/ttyUSB0
SKETCH = MQTT_4_Way_Mains_Power_Sensor_MLP201136.ino
LIBRARY_DIR = ../..

.PHONY: all compile upload clean deps

all: compile

deps:
	arduino-cli core update-index --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
	arduino-cli core install esp8266:esp8266 --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
	arduino-cli lib install "PubSubClient"
	arduino-cli lib install "ArduinoJson"

compile:
	arduino-cli compile --fqbn $(FQBN) --library $(LIBRARY_DIR) $(SKETCH)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

clean:
	arduino-cli cache clean
