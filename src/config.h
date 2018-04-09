#include "application.h"

#ifndef SPRINKLER_CONFIG_H
#define SPRINKLER_CONFIG_H 1


const pin_t SWITCH_PIN = D3;
const pin_t SPRINKLER_RELAY = D0;


const uint8_t statsAddr = 0;

struct Stats {
  uint8_t version; // 0
  uint32_t duration; // seconds
  int32_t deadline; // unix time (sunrise)
  int32_t targetStartTime; // unix time
  uint32_t cityID; // Open Weather API city ID
};

const Stats defaultStats = {
                      0, // Version 0
                      900, // 15 minutes
                      1522996200, // 4/6/18 @ 6:30am EST
                      1522995300, //4/6/18 @ 6:15am EST
                      5084633 //Claremont NH
                    };
#endif
