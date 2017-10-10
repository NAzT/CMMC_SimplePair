#ifndef CMMC_SimplePair_H
#define CMMC_SimplePair_H

#include "ESP8266WiFi.h"
#include <functional>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

enum CMMC_SimplePair_mode_t {
    MODE_AP, MODE_STA
};


class CMMC_SimplePair
{
public:
    CMMC_SimplePair_mode_t mode;

    // constructure
    CMMC_SimplePair() {}
    
    ~CMMC_SimplePair() {}
    
    void setup();

};

#endif //CMMC_SimplePair_H
