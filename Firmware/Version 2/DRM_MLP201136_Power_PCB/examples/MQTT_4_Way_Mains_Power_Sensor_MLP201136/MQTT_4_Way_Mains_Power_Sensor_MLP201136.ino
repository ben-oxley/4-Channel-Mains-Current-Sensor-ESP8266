#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
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
int8_t ADS_Input = 0;                             // A/D channel select
int8_t readings = 0;                             // A/D channel select
double Value[4] = {0, 0, 0, 0};                   // array for results
// mux connections
#define Select_A 4
#define Select_B 5
#define Cal_value 1500


unsigned long lastMillis = 0;
const long interval = 5000;
 
#define MQTT_PUBLISH_TOPIC   "esp8266/pub"
#define MQTT_SUBSCRIBE_TOPIC "esp8266/sub"
 
WiFiClient net;
PubSubClient client(net);
 
// library for the MLP201136 PCB
#include <MLP201136.h>
// make an instance of MLP201136
MLP201136 My_PCB(Select_A, Select_B, Cal_value);


// Custom default values
String My_MAC = "";                               // MAC address, tobe read from ESP8266
char MAC_array[13] = "000000000000";              // MAC definition
String My_IP = "";                                // IP address

 
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
 
 
void connectMQTT()
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

    } // end of loop

    // make a string of that IP address array
    My_IP = WiFi.localIP().toString();

    // make MAC address array used for Client ID
    My_MAC.toCharArray(MAC_array, (My_MAC.length() + 1));

    // print a connection report
    Serial.println("");
    Serial.print("WiFi connected, "); Serial.print("IP Address = "); Serial.println(WiFi.localIP());

  } // end of connected
 
  client.setServer(MQTT_HOST, 1883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Connecting to Local MQTT Broker");
 
  while (!client.connect(MAC_array, MQTT_USER, MQTT_PASSWORD))
  {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" Try again in 5 seconds");
    delay(5000);
  }
 
  if (!client.connected()) {
    Serial.println("MQTT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(MQTT_SUBSCRIBE_TOPIC);
 
  Serial.println("MQTT Connected!");
}
 
 
void publishMessage()
{

  // I/O
  pinMode(Run_LED, OUTPUT);
  pinMode(Network_LED, OUTPUT);
  
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
  doc["ip"] = WiFi.localIP().toString();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(MQTT_PUBLISH_TOPIC, jsonBuffer);
}
 
 
void setup()
{
  
  Serial.begin(115200);
  connectMQTT();
}
 
 
void loop()
{
  // read A/D values and store in array Value[]
  // these values are representations of Amps (RMS) measured, and still require some calibration
  digitalWrite(Run_LED, LOW);
  // sampling each channel takes around 400mS. 400 samples (20 cycles @50Hz) with a 1mS per A/D sample.
  // higher sampling rates can have issues when WiFi enabled on the ESP8266
  double value = My_PCB.power_sample(ADS_Input);
  Value[ADS_Input] = ((readings*Value[ADS_Input]) + value)/(readings+1);
  digitalWrite(Run_LED, HIGH);

  // inc ready for next A/D channel
  ADS_Input++;
  if (ADS_Input > 3) {
    ADS_Input = 0;
    readings++;
  } // end if
  
  if (!client.connected())
  {
    Serial.println("Connection lost, reconnecting");
    connectMQTT();
  }
  else
  {
    
    client.loop();
    if (millis() - lastMillis > 5000)
    {
      digitalWrite(Network_LED, HIGH);
      lastMillis = millis();
      Serial.println("Publishing");
      publishMessage();
      for (int i = 0; i < 4; i++){
        Serial.print(" Value ");Serial.print(i);Serial.print(": ");Serial.print(Value[i]);
        Value[i]=0; 
      }
      Serial.println("");
      readings = 0;
      Serial.println("Published");
    } else {
      digitalWrite(Network_LED, LOW);
    }

    
  }
}
