/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "Particle.h"
#include "config.h"
#include "publishMessage.h"

Stats SprinklerStats;

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

enum states_sprinkler {
  INIT, CHECK_SHOULD_BE_ON, TURN_SPRINKLER_ON, CHECK_DURATION_EXPIRED, TURN_SPRINKLER_OFF, ALLOW_SLEEP
};
enum states_sprinkler state = INIT;
enum states_sprinkler prevState = INIT;

uint32_t checkShouldBeOnTimeout = 30000;
uint32_t checkDurationExpiredTimeout = 30000;

bool FLAG_CanSleep = false;

enum sleep_types { WIFI_ONLY, DEEP };
enum sleep_types sleepType = DEEP;

enum sprinkler_states { ON, OFF }; // ON = 0, OFF = 1 handy trick as relay is "ON" when pin is "LOW"
retained enum sprinkler_states sprinkler_state = OFF;

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH_PIN,INPUT_PULLUP);
  pinMode(SPRINKLER_RELAY, OUTPUT);
  digitalWrite(SPRINKLER_RELAY, sprinkler_state);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    SprinklerStats = defaultStats;
    EEPROM.put(statsAddr, SprinklerStats);
  }
  Time.zone(-4);
  Particle.subscribe("hook-response/getSunrise", getSunriseResponseHandler, MY_DEVICES);

  bool FLAG_CanSleep = false;
  state = CHECK_SHOULD_BE_ON;
}


void loop() {
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

  switch(state){
    case INIT:
      break;

    case CHECK_SHOULD_BE_ON:
      if(Time.now() >= SprinklerStats.targetStartTime){
        if(Time.now() <= SprinklerStats.deadline) state = TURN_SPRINKLER_ON;
        if(Time.now() >= SprinklerStats.deadline) state = TURN_SPRINKLER_OFF;
      }

      static uint32_t checkShouldBeOnTimer;
      if(checkShouldBeOnTimer == 0) checkShouldBeOnTimer = millis();
      if(millis() - checkShouldBeOnTimer > checkShouldBeOnTimeout) state = ALLOW_SLEEP;

      break;

    case TURN_SPRINKLER_ON:
      if(sprinkler_state == OFF){
        publishMessage("googleDocs","Turning sprinkler ON");
      }
      sprinkler_state = ON;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
      state = CHECK_DURATION_EXPIRED;
      break;

    case CHECK_DURATION_EXPIRED:
      if(Time.now() >= SprinklerStats.deadline) state = TURN_SPRINKLER_OFF;

      static uint32_t checkDurationExpiredTimer;
      if(checkDurationExpiredTimer == 0) checkDurationExpiredTimer = millis();
      if(millis()-checkDurationExpiredTimer > checkDurationExpiredTimeout) state = ALLOW_SLEEP;

      break;

    case TURN_SPRINKLER_OFF:
      if(sprinkler_state == ON){
        publishMessage("googleDocs","Turning sprinkler OFF");
      }
      sprinkler_state = OFF;
      digitalWrite(SPRINKLER_RELAY, sprinkler_state);
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

	if (cloudReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
      getSunriseTime(SprinklerStats.cityID, NO_PROCESS); // Get the current sunrise time from Open Weather API, arg is city ID
      if(!digitalRead(SWITCH_PIN)){
        publishMessage("general_message","Swith ON", NO_PROCESS);
      } else {
        publishMessage("general_message", "Switch OFF", NO_PROCESS);
      }
		}
		if ((millis() - firstAvailable > 30000) && FLAG_CanSleep) {
      Particle.process();
			System.sleep(SLEEP_MODE_DEEP,30);
		}
	}
	else {
		firstAvailable = 0;
	}
}
