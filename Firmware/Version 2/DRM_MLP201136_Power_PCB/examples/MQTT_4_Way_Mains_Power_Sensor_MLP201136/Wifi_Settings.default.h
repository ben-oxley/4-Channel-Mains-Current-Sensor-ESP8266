/*
    Wifi settings and IP address
*/

// select Static IP Mode, uncomment below to enable
#ifdef Fixed_IP
#define STATIC_IP
#endif
IPAddress ip(192, 168, 0, 199);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Wifi access point and passwords
const char* SSID_1 = "ssid";
const char* Password_1 = "password";

const char* SSID_2 = "ssid";
const char* Password_2 = "password";

const char* SSID_3 = "ssid";
const char* Password_3 = "password";

const char* SSID_4 = "ssid";
const char* Password_4 = "password";
 
 
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

 
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----

 
)KEY";
