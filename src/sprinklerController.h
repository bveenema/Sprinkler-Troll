#pragma once

#include "application.h"
#include "config.h"
#include "publishMessage.h"

extern Stats SprinklerStats;

// ON = 0, SPRINKLER_OFF = 1 handy trick as relay is "ON" when pin is "LOW"
enum sprinkler_states { SPRINKLER_ON, SPRINKLER_OFF };

extern retained enum sprinkler_states sprinkler_state;

class SprinklerController{
public:
  SprinklerController();

  void begin(void);

  void update(void);

  enum sleep_types canSleep(void);

private:
  bool FLAG_CanSleep = false;
  const uint32_t checkShouldBeOnTimeout = 15000;
  const uint32_t checkDurationExpiredTimeout = 15000;

  enum states_sprinkler {
    INIT, CHECK_SHOULD_BE_ON, TURN_SPRINKLER_ON, CHECK_DURATION_EXPIRED, TURN_SPRINKLER_OFF, ALLOW_SLEEP
  };
  enum states_sprinkler state = INIT;

  enum sleep_types sleepType = DEEP;
};
