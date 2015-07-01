// WiFi demo
// Tested on an ESP8266 ESP-07
// (The Arduino WiFi shield uses the same functions, just change the include)

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap.h>
#include <WiFi+coap.h>

const char* ssid = "_________";
const char* password = "_________";

void setup(void) {
  Serial.begin(115200);
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IPv4: ");
  Serial.println(WiFi.localIP());
  coap_wifi_begin();
}

void loop(void) {
  coap_wifi_loop();
  delay(50);
}

#define ROUTES \
ROUTE(buzzer, COAP_METHOD_GET, URL("demo"), ";if=\"test\"", { \
  char rspc[3] = "OK"; \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
})

#include <conatra.h>
