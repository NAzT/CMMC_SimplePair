#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CMMC_SimplePair.h>

CMMC_SimplePair instance;

void dump(const u8* data, size_t size) {
  for (size_t i = 0; i < size-1; i++) {
    Serial.printf("%02x ", data[i]);
  }
  Serial.printf("%02x", data[size-1]);
  Serial.println();
}

void evt_success(u8* sa, u8 status, const u8* key) {
  Serial.printf("[CSP_EVENT_SUCCESS] STATUS: %d\r\n", status);
  Serial.printf("WITH KEY: "); dump(key, 16);
  Serial.printf("WITH MAC: "); dump(sa, 6);
}

void evt_error(u8* sa, u8 status, const char* cause) {
  Serial.printf("[CSP_EVENT_ERROR] %d: %s\r\n", status, cause);
}

void setup()
{
  u8 pair_key[16] = {0x09, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  u8 message[16] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0xfa};

  Serial.begin(115200);

  instance.begin(MASTER_MODE, pair_key, message, evt_success, evt_error);
  instance.add_debug_listener([](const char* s) {
    Serial.printf("[USER]: %s\r\n", s);
  });
  instance.start();
}

void loop()
{

}
