/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "Particle.h"
#include "sprinkler-troll-util.h"
#include "config.h"
#include "sprinklerController.h"
#include "CloudManager.h"
#include <PublishManager.h>

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

PublishManager publishManager;
CloudManager cloudManager;
Stats SprinklerStats;
SprinklerController sprinklerController;

// Retained variable: DO NOT change order, type or anything without a complete VIN and VBAT power cycle
retained enum sprinkler_states sprinkler_state;

void setup() {
  Serial.begin(115200);

  sprinklerController.begin();
  cloudManager.begin();

  pinMode(SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    SprinklerStats = defaultStats;
    EEPROM.put(statsAddr, SprinklerStats);
  }
  Time.zone(-4);
}


void loop() {
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

  sprinklerController.update();

	if (cloudReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
      cloudManager.onConnect(sprinkler_state);
		}
		if ((millis() - firstAvailable > wakeTime*1000) && sprinklerController.canSleep() != NO_SLEEP) {
      Serial.printlnf("Time Awake: %u", millis()-firstAvailable);
      uint32_t timeToSleep = determineSleepTime(timeOfDay(Time.now()), SprinklerStats.targetStartTime, SprinklerStats.deadline);
      publishManager.publish("Time To Sleep", String(timeToSleep).c_str());
      Particle.process();
      if(sprinklerController.canSleep() == DEEP){
        System.sleep(SLEEP_MODE_DEEP,30);
      } else {
        System.sleep(30);
      }

		}
	}
	else {
		firstAvailable = 0;
	}

  static uint32_t printTime;
  if(millis() - printTime > 1000){
    printTime = millis();
    Serial.printlnf("I'm Alive: %u", printTime);
  }
}
