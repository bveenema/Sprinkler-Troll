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
#include <ParticlePromise.h>

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

PublishManager<> publishManager;
ParticlePromise promise;
cloudManager CloudManager;
Stats SprinklerStats;
SprinklerController sprinklerController;

bool serialReady  = false;

// Retained variable: DO NOT change order, type or anything without a complete VIN and VBAT power cycle
retained enum sprinkler_states sprinkler_state;

void setup() {
  Serial.begin(115200);

  sprinklerController.begin();
  CloudManager.begin();

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
  serialReady = Serial.isConnected();

	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

  sprinklerController.update();
  CloudManager.update();

	if (cloudReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
      CloudManager.onConnect(sprinkler_state);
		}
		if ((millis() - firstAvailable > wakeTime*1000) && sprinklerController.canSleep() != NO_SLEEP) {
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
}
