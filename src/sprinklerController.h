#pragma once

#include "application.h"
#include "config.h"
#include "CloudManager.h"

extern Stats SprinklerStats;
extern CloudManager cloudManager;

extern retained enum sprinkler_states sprinkler_state;

class SprinklerController{
public:
  SprinklerController();

  void begin(void);

  void update(void);

  enum sleep_types canSleep(void);

private:
  bool FLAG_CanSleep = false;
  const uint32_t timeout = 15000;

  enum states_sprinkler {
    INIT, CHECK_SPRINKLER_STATE, TURN_SPRINKLER_ON, CHECK_DURATION_EXPIRED, TURN_SPRINKLER_OFF, ALLOW_SLEEP
  };
  enum states_sprinkler state = INIT;

  enum sleep_types sleepType = DEEP;
};
