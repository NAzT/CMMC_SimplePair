#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CMMC_SimplePair.h>

CMMC_SimplePair instance;

void setup()
{
  Serial.begin(115200);
  Serial.println("HELLO from STA");

  instance.begin(CSP_MODE_STA);
}

void loop()
{

}
