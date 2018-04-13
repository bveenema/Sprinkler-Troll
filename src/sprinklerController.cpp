#include "sprinklerController.h"

SprinklerController::SprinklerController(){
  FLAG_CanSleep = false;
  sleepType = DEEP;
  state = INIT;
}

void SprinklerController::begin(void){
  pinMode(SPRINKLER_RELAY, OUTPUT);
  digitalWrite(SPRINKLER_RELAY, sprinkler_state);
  state = CHECK_SHOULD_BE_ON;
}

void SprinklerController::update(void){

  switch(state){
    case INIT:
      break;

    case CHECK_SHOULD_BE_ON: {
        uint32_t timeNow = timeOfDay(Time.now());

        if(timeNow >= SprinklerStats.targetStartTime && timeNow <= SprinklerStats.deadline) state = TURN_SPRINKLER_ON;
        else {
          static uint32_t checkShouldBeOnTimer;
          if(checkShouldBeOnTimer == 0) checkShouldBeOnTimer = millis();
          if(millis() - checkShouldBeOnTimer > checkShouldBeOnTimeout) state = TURN_SPRINKLER_OFF;
        }
      }
      break;

    case TURN_SPRINKLER_ON:
      if(sprinkler_state == SPRINKLER_OFF){
        publishMessage("googleDocs","Turning sprinkler ON");
      }
      sprinkler_state = SPRINKLER_ON;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
      sleepType = WIFI_ONLY;
      state = CHECK_DURATION_EXPIRED;
      break;

    case CHECK_DURATION_EXPIRED:
      if(timeOfDay(Time.now()) >= SprinklerStats.deadline) state = TURN_SPRINKLER_OFF;

      static uint32_t checkDurationExpiredTimer;
      if(checkDurationExpiredTimer == 0) checkDurationExpiredTimer = millis();
      if(millis()-checkDurationExpiredTimer > checkDurationExpiredTimeout) state = ALLOW_SLEEP;

      break;

    case TURN_SPRINKLER_OFF:
      if(sprinkler_state == SPRINKLER_ON){
        publishMessage("googleDocs","Turning sprinkler OFF");
      }
      sprinkler_state = SPRINKLER_OFF;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
      sleepType = DEEP;
      state = ALLOW_SLEEP;
      break;

    case ALLOW_SLEEP:
      FLAG_CanSleep = true;
      break;

    default:
      char buffer[100];
      sprintf(buffer, "ERROR: State machine bad state, %i", state);
      publishMessage("googleDocs",buffer,PROCESS);
      state = CHECK_SHOULD_BE_ON;
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
