#pragma once

#include "application.h"

#ifndef SPRINKLER_CONFIG_H
#define SPRINKLER_CONFIG_H


const pin_t SWITCH_PIN = D3;
const pin_t SPRINKLER_RELAY = D0;

const uint32_t maxDuration = 10800;// 3 hours -> 180 minutes -> 10800 seconds
const uint32_t sleepTime = 3600; // Maximum time to sleep (seconds)
const uint32_t wakeTime = 30; // Minimum time to be awake (seconds)


const uint8_t statsAddr = 0;

struct Stats {
  uint8_t version; // 0
  uint32_t duration; // seconds
  uint32_t deadline; // seconds from midnight (sunrise)
  uint32_t targetStartTime; // seconds from midnight
  uint32_t cityID; // Open Weather API city ID
};

const Stats defaultStats = {
                      0, // Version 0
                      900, // 15 minutes
                      37800, // 6:30am EST
                      36900, // 6:15am EST
                      5084633 //Claremont NH
                    };

enum sleep_types { NO_SLEEP, WIFI_ONLY, DEEP };

// ON = 0, SPRINKLER_OFF = 1 handy trick as relay is "ON" when pin is "LOW"
enum sprinkler_states { SPRINKLER_ON, SPRINKLER_OFF };

#endif
