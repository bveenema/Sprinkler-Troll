/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "publishMessage.h"
#include <ArduinoJson.h>

const pin_t SWITCH_PIN = D3;

const char* sprinklerTimeMessage = "{\"duration\":1200,\"deadline\":1522919753}";

//TODO
uint8_t statsAddr = 0;
struct Stats {
  uint8_t version; // 0
  uint32_t duration; // seconds
  uint32_t deadline; // unix time (sunrise)
  uint32_t targetStartTime; // unix time
  uint32_t cityID; // Open Weather API city ID
};
Stats SprinklerStats;

void setup() {
  pinMode(SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    Stats defaultStats = { 0, 900, 1522996200, 1522995300, 5084633}; // 15 minutes, 4/6/18 @ 6:30am EST, 4/6/18 @ 6:15am EST, Claremont NH
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
