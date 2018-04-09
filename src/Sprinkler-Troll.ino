/*
 * Project Sprinkler-Troll
 * Description: Sprinkler Controller
 * Author: Veenema Design Works
 */
#include "Particle.h"
#include "config.h"
#include "publishMessage.h"


void setup() {
  pinMode(SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(statsAddr, SprinklerStats);
  if(SprinklerStats.version != 0){
    // EEPROM was empty -> initialize Stats
    SprinklerStats = defaultStats;
    EEPROM.put(statsAddr, SprinklerStats);
  }
  Time.zone(-4);
  Particle.subscribe("hook-response/getSunrise", getSunriseResponseHandler, MY_DEVICES);
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
