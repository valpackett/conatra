// IoT Weather Station + Fire Alarm
// Tested on a Leonardo + ENC28J60

#include <stdlib.h>
#include <SPI.h>
#include <Wire.h>

//// Network Config ////

#include <EtherCard.h>
#include <coap.h> // For some reason, must be included before the sensor stuff?!!?!?!
#include <EtherCard+coap.h>

byte Ethernet::buffer[400]; // If the program hangs, try increasing this
static uint8_t mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

//// Sensor Config ////

#define FLAME_PIN             0     // analog
#define FLAME_ALERT_THRESHOLD 100

#define MQ2_PIN               2     // analog
#define MQ2_ALERT_THRESHOLD   115

#define PIR_PIN               8     // digital

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHT_PIN  0 // digital
#define DHT_TYPE DHT11
DHT_Unified dht(DHT_PIN, DHT_TYPE);

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

//// Output Config ////

#define BUZZER_PIN  5
#define BUZZER_HIGH 30

//// Data Memory ////

static uint8_t  temp;
static uint8_t  humidity;
static uint8_t  flame;
static uint8_t  mq2_gas;
static bool     motion;
static float    pressure;

static uint32_t timer;
static uint32_t last_alert_time;

//// Setup & Loop ////

void setup(void) {
  last_alert_time = millis() + 20 * 1000; // Wait for the MQ2 to stabilize, do not beep
  bmp.begin();
  dht.begin();
  if (ether.begin(sizeof Ethernet::buffer, mymac, 4) == 0)
    Serial.println(F("EthErr"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCPErr"));
  else
    ether.printIp("My IP: ", ether.myip);
  coap_ethercard_begin();
  coap_ethercard_begin_multicast();
}

void loop(void) {
  timer = millis();
  timer -= timer % 100;
  if (timer % 10 == 0)
    ether.packetLoop(ether.packetReceive());
  if (timer % 100 == 0) {
    readAnalogSensors();
    writeAnalogOutputs();
  }
  if (timer % 2000 == 0) {
    readDigitalSensors();
  }
  delay(50);
}

void readDigitalSensors() {
  motion = digitalRead(PIR_PIN) == HIGH;
  
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  temp = event.temperature;
  if (isnan(event.temperature))
    Serial.println(F("TempErr"));

  dht.humidity().getEvent(&event);
  humidity = event.relative_humidity;
  if (isnan(event.relative_humidity))
    Serial.println(F("HumidErr"));

  bmp.getEvent(&event);
  pressure = event.pressure * 0.75; // hPa -> mmHg
  float temperature;
  bmp.getTemperature(&temperature);
  temp += temperature;
  temp /= 2; // Average between two sensors
}

void readAnalogSensors() {
  flame = analogRead(FLAME_PIN);
  mq2_gas = analogRead(MQ2_PIN);
}

void writeAnalogOutputs() {
  analogWrite(BUZZER_PIN, 0);
  if ((flame > FLAME_ALERT_THRESHOLD || mq2_gas > MQ2_ALERT_THRESHOLD)
      && millis() - last_alert_time > 1000) {
    beep();
    last_alert_time = millis();
  }
}

void beep() {
  analogWrite(BUZZER_PIN, BUZZER_HIGH);
  delay(80);
  analogWrite(BUZZER_PIN, 0);
  delay(20);
}

//// Network Endpoints ////

#define ROUTES \
ROUTE(temperature, COAP_METHOD_GET, URL("temperature"), ";if=\"sensor\";rt=\"c\"", { \
  char rspc[3]; itoa(temp, rspc, 10); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(humidity, COAP_METHOD_GET, URL("humidity"), ";if=\"sensor\";rt=\"percent\"", { \
  char rspc[3]; itoa(humidity, rspc, 10); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(pressure, COAP_METHOD_GET, URL("pressure"), ";if=\"sensor\";rt=\"mmhg\"", { \
  char rspc[7]; dtostrf(pressure, 6, 2, rspc); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(flame, COAP_METHOD_GET, URL("flame"), ";if=\"sensor\"", { \
  char rspc[3]; itoa(flame, rspc, 10); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(gas, COAP_METHOD_GET, URL("gas"), ";if=\"sensor\"", { \
  char rspc[5]; itoa(mq2_gas, rspc, 10); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(motion, COAP_METHOD_GET, URL("motion"), ";if=\"sensor\"", { \
  char rspc[2];  itoa(motion, rspc, 10); \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
}) \
ROUTE(buzzer, COAP_METHOD_POST, URL("buzzer"), ";if=\"alarm\"", { \
  beep(); \
  char rspc[3] = "OK"; \
  CONTENT(COAP_CONTENTTYPE_TEXT_PLAIN, rspc); \
})

#include <conatra.h>
