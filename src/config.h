#pragma once

#include "application.h"

#ifndef SPRINKLER_CONFIG_H
#define SPRINKLER_CONFIG_H


const pin_t SWITCH_PIN = D3;
const pin_t SPRINKLER_RELAY = D0;

const int maxDuration = 10800;// 3 hours -> 180 minutes -> 10800 seconds
const int sleepTime = 3600; // Maximum time to sleep (seconds)
const int wakeTime = 30; // Minimum time to be awake (seconds)


const uint8_t statsAddr = 0;

const uint8_t maxCityCharacters = 25;
const uint8_t maxStateCharacters = 25; // "state" is used in the USA-centric context such as California and in the self-governing body context, such as "Russia"

struct Stats {
  uint8_t version; // 0
  int duration; // seconds
  int deadline; // seconds from midnight (sunrise)
  int targetStartTime; // seconds from midnight
  int timeZone;
  char stateName[maxStateCharacters];
  char cityName[maxCityCharacters];
};

const Stats defaultStats = {
                      0, // Version 0
                      900, // 15 minutes
                      37800, // 6:30am EST
                      36900, // 6:15am EST
                      -5, // EDT
                      "VT",
                      "Hartland"
                    };

enum sleep_types { NO_SLEEP, WIFI_ONLY, DEEP };

// ON = 0, SPRINKLER_OFF = 1 handy trick as relay is "ON" when pin is "LOW"
enum sprinkler_states { SPRINKLER_ON, SPRINKLER_OFF };

#endif
