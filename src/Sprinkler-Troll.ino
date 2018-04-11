/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "Particle.h"
#include "sprinkler-troll-util.h"
#include "config.h"
#include "sprinklerController.h"
#include "publishMessage.h"
#include <PublishManager.h>

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

PublishManager publishManager;
Stats SprinklerStats;
SprinklerController sprinklerController;

retained enum sprinkler_states sprinkler_state;

void setup() {
  Serial.begin(115200);

  sprinklerController.begin();

  pinMode(SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    SprinklerStats = defaultStats;
    EEPROM.put(statsAddr, SprinklerStats);
  }
  Time.zone(-4);

  Particle.subscribe("hook-response/getSunrise", getSunriseResponseHandler, MY_DEVICES);
  Particle.subscribe("hook-response/getGoogleDocs", getGoogleDocsResponseHandler, MY_DEVICES);
  Particle.publish("getGoogleDocs",NULL,60,PRIVATE);
}


void loop() {
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

  sprinklerController.update();

	if (cloudReady) {
		if (firstAvailable == 0) {
			firstAvailable = millis();
      getSunriseTime(SprinklerStats.cityID, NO_PROCESS);
		}
		if ((millis() - firstAvailable > 30000) && sprinklerController.canSleep()) {
      Particle.process();
			System.sleep(SLEEP_MODE_DEEP,30);
		}
	}
	else {
		firstAvailable = 0;
	}
}

void getGoogleDocsResponseHandler(const char *event, const char *data) {
  publishMessage("G-Docs_Get_Response", data);
}
