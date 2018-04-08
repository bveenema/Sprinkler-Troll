/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "Particle.h"
#include "config.h"
#include "publishMessage.h"
#include <ArduinoJson.h>

//const char* sprinklerTimeMessage = "{\"duration\":1200,\"deadline\":1522919753}";

void setup() {
  pinMode(SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    SprinklerStats = defaultStats;
    EEPROM.put(statsAddr, SprinklerStats);
  }
  Time.zone(-4);
  Particle.subscribe("hook-response/getSunrise", getSunriseHandler, MY_DEVICES);
}


void loop() {
	bool cloudReady = Particle.connected();
  static uint32_t firstAvailable;

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
		if (millis() - firstAvailable > 30000) {
      Particle.process();
			System.sleep(SLEEP_MODE_DEEP,30);
		}
	}
	else {
		firstAvailable = 0;
	}
}

void parseMessage(const char* message){
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 40;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(message);

  SprinklerStats.duration = root["duration"]; // 1200
  SprinklerStats.deadline = root["deadline"]; // 1522919753
}

void getSunriseHandler(const char *event, const char *data) {
  uint32_t newDeadline = atoi(data);

  if(newDeadline != SprinklerStats.deadline){
    SprinklerStats.deadline = newDeadline;
    SprinklerStats.targetStartTime = newDeadline - SprinklerStats.duration;
    EEPROM.put(statsAddr, SprinklerStats);
    char buffer[100];
    sprintf(buffer, "new DEADLINE: %i", SprinklerStats.deadline);
    publishMessage("googleDocs", buffer, PROCESS);
  }

  Particle.publish("response",Time.timeStr(newDeadline),60,PRIVATE);
}
