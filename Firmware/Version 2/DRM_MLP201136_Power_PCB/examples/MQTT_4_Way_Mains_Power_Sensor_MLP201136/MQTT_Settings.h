/*
      MQTT Broker and topic
*/


// MQTT Broker, change as required
const char* mqtt_server = "a2q9hwffnw3xq5-ats.iot.eu-west-2.amazonaws.com";

// MQTT Topics
const char* InStatus = "Mains_Power/Status";
const char* InControl = "Mains_Power/Control";

// the ESP8266's MAC address is normally used to send a message to a selected device. 
// below is the address used to broadcast to all devices subscribed to the above topic.
String Broadcast_All = "*ALL";
