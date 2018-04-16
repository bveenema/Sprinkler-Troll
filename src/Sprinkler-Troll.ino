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

// Retained variable: DO NOT change order, type or anything without a complete VIN and VBAT power cycle
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
		if ((millis() - firstAvailable > 30000) && sprinklerController.canSleep() != NO_SLEEP) {
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

void getGoogleDocsResponseHandler(const char *event, const char *data) {

  const size_t bufferSize = JSON_OBJECT_SIZE(4) + 80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(data);

  uint32_t newDuration = root["duration"];
  uint32_t newCityID = root["cityID"];
  // const char* name = root["name"];
  // const char* coreid = root["coreid"];

  // Check that newCityID is 6 or 7 digit integer
  if(newCityID >= 100000 && newCityID <= 9999999){
    if(newCityID != SprinklerStats.cityID){
      char buffer[100];
      sprintf(buffer, "new City ID: %i", newCityID);
      publishMessage("googleDocs", buffer);
      SprinklerStats.cityID = newCityID;
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID CITY ID: %i", newCityID);
    publishMessage("googleDocs", buffer);
  }

  // Check that duration is valid number
  if(newDuration < maxDuration){
    if(newDuration != SprinklerStats.duration){
      char buffer[100];
      sprintf(buffer, "new DURATION: %i", newDuration);
      publishMessage("googleDocs", buffer);
      SprinklerStats.duration = newDuration;
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID DURATION: %i", newDuration);
    publishMessage("googleDocs", buffer);
  }
}
