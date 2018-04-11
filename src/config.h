#pragma once

#include "application.h"

#ifndef SPRINKLER_CONFIG_H
#define SPRINKLER_CONFIG_H


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


// ON = 0, SPRINKLER_OFF = 1 handy trick as relay is "ON" when pin is "LOW"
enum sprinkler_states { SPRINKLER_ON, SPRINKLER_OFF };

// Retained variable: DO NOT change order, type or anything without a complete VIN and VBAT power cycle
extern retained enum sprinkler_states sprinkler_state;

#endif
