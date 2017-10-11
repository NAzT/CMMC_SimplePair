#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CMMC_SimplePair.h>

CMMC_SimplePair instance;

void setup()
{
  u8 key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

  Serial.begin(115200);
  Serial.println("HELLO...");

  instance.begin(CSP_MODE_AP, key);
  // instance.add_listener([](u8 *sa, u8 status) {
  //     Serial.printf("[USER] event %d\r\n", status);
  // });

  instance.on(CSP_EVENT_SUCCESS, [](u8* sa, u8 status) {
    instance.mode();
    Serial.printf("[USER][EVENT_SUCCESS] %d\r\n", status);
  });

  instance.on(CSP_EVENT_ERROR, [](u8* sa, u8 status) {
    Serial.printf("[USER][EVENT_ERROR] %d\r\n", status);
  });

  instance.start();

}

void loop()
{

}
