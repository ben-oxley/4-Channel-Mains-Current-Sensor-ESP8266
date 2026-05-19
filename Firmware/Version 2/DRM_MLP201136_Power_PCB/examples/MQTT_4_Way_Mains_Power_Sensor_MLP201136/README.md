# MQTT 4-Way Mains Power Sensor (Local Broker)

This example demonstrates how to read current values from the MLP201136 PCB and publish them to a local MQTT broker over WiFi.

## Prerequisites

To build and flash this firmware from the command line on Linux, you will need the `arduino-cli` tool.

1. Install `arduino-cli` by following the [official instructions](https://arduino.github.io/arduino-cli/latest/installation/).
2. You can install the required dependencies (ESP8266 core and libraries) by running:
   ```bash
   make deps
   ```

## Configuration

Before compiling and flashing, edit `secrets.h` and add your WiFi and MQTT credentials:

```c
#define SECRET

const char WIFI_SSID[] = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";

const char MQTT_HOST[] = "192.168.1.100";
const char MQTT_USER[] = "your_username";
const char MQTT_PASSWORD[] = "your_password";
```

## Building and Flashing

A `Makefile` is provided to simplify using `arduino-cli`.

1. Connect your ESP8266 to your computer via USB.
2. Find the serial port (usually `/dev/ttyUSB0` or `/dev/ttyACM0`). You can use `ls /dev/ttyUSB*` to check.
3. Compile and upload the sketch using the following command (replace `/dev/ttyUSB0` if your port is different):

```bash
make upload PORT=/dev/ttyUSB0
```

### Other Commands

- **Compile only (without uploading):**
  ```bash
  make compile
  ```

- **Clean build cache:**
  ```bash
  make clean
  ```

### Advanced Usage

If you are using a different ESP8266 board type, you can override the `FQBN` (Fully Qualified Board Name) variable. The default is `esp8266:esp8266:nodemcuv2`.

```bash
make upload PORT=/dev/ttyUSB0 FQBN=esp8266:esp8266:generic
```
