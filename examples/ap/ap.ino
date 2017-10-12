#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CMMC_SimplePair.h>

CMMC_SimplePair instance;

void evt_success(u8* sa, u8 status, const u8* key) {
  Serial.printf("[CSP_EVENT_SUCCESS] STATUS: %d WITH KEY => \r\n", status);
  for (size_t i = 0; i < 16; i++) {
    Serial.printf("%02x ", key[i]);
  }
}

void evt_error(u8* sa, u8 status, const char* cause) {
  Serial.printf("[CSP_EVENT_ERROR] %d\r\n", status);
}

void setup()
{
  u8 key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  instance.begin(MASTER_MODE, key, evt_success, evt_error);
  instance.on(EVENT_SUCCESS, evt_success);
  instance.on(EVENT_ERROR,  evt_error);
  instance.add_debug_listener([](const char* s) {
    Serial.printf("[USER]: %s\r\n", s);
  });
  instance.start();
}

void loop()
{

}
