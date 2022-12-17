#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"

// ********************************************************************************************************
// ************************ project specific variables ****************************************************
// ********************************************************************************************************

// I/O and product variables
#define Speaker 13
#define Network_LED 14

// Run LED
#define Run_LED 16

// MLP201136 and A/D items
double Value[4] = {0, 0, 0, 0};                   // array for results
// mux connections
#define Select_A 4
#define Select_B 5
#define Cal_value 1500


unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
 
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"
 
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);
 
time_t now;
time_t nowish = 1510592825;

// library for the MLP201136 PCB
#include <MLP201136.h>
// make an instance of MLP201136
MLP201136 My_PCB(Select_A, Select_B, Cal_value);

// Custom default values
String WiFi_SSID = "None";                        // SSID string
String My_MAC = "";                               // MAC address, tobe read from ESP8266
char MAC_array[13] = "000000000000";              // MAC definition
String My_IP = "";                                // IP address
 
 
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
 
 
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  // if connected
  if (WiFi.status() == WL_CONNECTED) {

    // get MAC address, 6 characters in an array
    byte mac[6];  WiFi.macAddress(mac);

    My_MAC = "";

    // make a string of the MAC with colons "0" padding and upper case results
    // the MAC address is used as the products MQTT device address
    for (int i = 0; i <= 5; i++) {

      // add leading zero if missing
      String M = String(mac[i], HEX); M.toUpperCase();
      if (M.length() < 2) {
        M = "0" + M;
      } // end of padding

      My_MAC = My_MAC + M;

      // add colon of required
      if (i < 5) {
        My_MAC = My_MAC;
      }

    } // end of loop

    // make a string of that IP address array
    My_IP = WiFi.localIP().toString();

    // make MAC address array used for Client ID
    My_MAC.toCharArray(MAC_array, (My_MAC.length() + 1));

    // print a connection report
    Serial.println("");
    Serial.print("WiFi connected, "); Serial.print("DHCP IP Address = "); Serial.println(WiFi.localIP());
    // Serial.print(", "); Serial.println(Status_Report());

  } // end of connected
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
 
void publishMessage()
{
  // WiFi Version
  long rssi = WiFi.RSSI();
  
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["value0"] = Value[0];
  doc["value1"] = Value[1];
  doc["value2"] = Value[2];
  doc["value3"] = Value[3];
  doc["rssi"] = rssi;
  doc["mac"] = My_MAC;
  doc["ssid"] = WiFi_SSID;
  doc["ip"] = WiFi.localIP().toString();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
 
void setup()
{
  
  Serial.begin(115200);
  connectAWS();
}
 
 
void loop()
{
  Serial.println("Starting read");
  // read A/D values and store in array Value[]
  // these values are representations of Amps (RMS) measured, and still require some calibration
  digitalWrite(Run_LED, LOW);
  for (int i = 0; i <4; i++){
  // sampling each channel takes around 400mS. 400 samples (20 cycles @50Hz) with a 1mS per A/D sample.
  // higher sampling rates can have issues when WiFi enabled on the ESP8266
    Value[i] = My_PCB.power_sample(i);
    ESP.wdtFeed();
  }
  digitalWrite(Run_LED, HIGH);
  Serial.println("Read Done");
  
  now = time(nullptr);
 
  if (!client.connected())
  {
    Serial.println("Connection lost, reconnecting");
    connectAWS();
  }
  else
  {
    digitalWrite(Network_LED, HIGH);
    client.loop();
    Serial.println("Time since: "); Serial.print(millis() - lastMillis > 5000);
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      Serial.println("Publishing");
      publishMessage();
      Serial.println("Published");
    }
    // only used to make the LED flash visable
    delay(10);

    digitalWrite(Network_LED, LOW);
  }
}
