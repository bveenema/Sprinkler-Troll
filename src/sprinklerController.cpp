#include "sprinklerController.h"

SprinklerController::SprinklerController(){
  FLAG_CanSleep = false;
  sleepType = DEEP;
  state = INIT;
}

void SprinklerController::begin(void){
  pinMode(SPRINKLER_RELAY, OUTPUT);
  digitalWrite(SPRINKLER_RELAY, sprinkler_state);
  state = CHECK_SPRINKLER_STATE;
  if(serialReady) Serial.printlnf("SprinklerState: %u",state);
}

void SprinklerController::update(void){
  static enum states_sprinkler prevState = state;

  switch(state){
    case INIT:
      CloudManager.log("State is INIT");
      delay(2000);
      this->begin();
      break;

    case CHECK_SPRINKLER_STATE: {
      uint32_t timeNow = timeOfDay(Time.now());

      // if startTime and deadline are in same day
      if(SprinklerStats.targetStartTime <= SprinklerStats.deadline){
        if(timeNow >= SprinklerStats.targetStartTime && timeNow <= SprinklerStats.deadline){
          state = TURN_SPRINKLER_ON;
        }else{
          state = TURN_SPRINKLER_OFF;
        }
      }else { // if startTime is previous day from deadline
        if(timeNow >= SprinklerStats.targetStartTime || timeNow <= SprinklerStats.deadline) {
          state = TURN_SPRINKLER_ON;
        }else{
          state = TURN_SPRINKLER_OFF;
        }
      }
      break;
    }


    case TURN_SPRINKLER_ON:
      if(sprinkler_state == SPRINKLER_OFF){
        char buffer[40];
        sprintf(buffer, "Turning sprinkler ON: %u", timeOfDay(Time.now()));
        CloudManager.log(buffer);
      }
      sprinkler_state = SPRINKLER_ON;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
      sleepType = WIFI_ONLY;
      state = ALLOW_SLEEP;
      break;

    case TURN_SPRINKLER_OFF:
      if(sprinkler_state == SPRINKLER_ON){
        char buffer[40];
        sprintf(buffer, "Turning sprinkler OFF: %u", timeOfDay(Time.now()));
        CloudManager.log(buffer);
      }
      sprinkler_state = SPRINKLER_OFF;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
      sleepType = DEEP;
      state = ALLOW_SLEEP;
      break;

    case ALLOW_SLEEP:
      static uint32_t timeoutTimer;
      if(timeoutTimer == 0) timeoutTimer = millis();
      if(millis() - timeoutTimer > timeout){
        FLAG_CanSleep = true;
        timeoutTimer = millis();
      }

      state = CHECK_SPRINKLER_STATE;

      break;

    default:
      char buffer[100];
      sprintf(buffer, "ERROR: State machine bad state, %i", state);
      CloudManager.log(buffer);
      state = CHECK_SPRINKLER_STATE;
      break;
  }
}

enum sleep_types SprinklerController::canSleep(void){
  if(FLAG_CanSleep) {
    return sleepType;
  } else {
    return NO_SLEEP;
  }
}
