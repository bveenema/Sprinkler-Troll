// CloudManager.h
// Purpose: Manage the cloud connection
//            1. Signal the main process when all cloud comms are complete - canSleep()
//            2. Make all API calls - onConnect
//            3. Handle Errors/Timeouts - ParticlePromise
//            4. Update SprinklerStats and EEPROM with new configuration data
//            5. Provide general message publishing function to other processes - message()
//            6. Provide Cloud logging function to other processes - log()

#pragma once

#include "application.h"
#include "sprinkler-troll-util.h"
#include "config.h"
#include <ArduinoJson.h>
#include <PublishManager.h>
#include <ParticlePromise.h>

extern PublishManager<> publishManager;
extern ParticlePromise promise;
extern Stats SprinklerStats;

class cloudManager {
public:
  void begin(){
    promise.enable();
  }

  void onConnect(enum sprinkler_states state);

  void publishStats();

  bool canSleep(){
    if(FLAG_rainForecastComplete && FLAG_sunriseForecastComplete && FLAG_getDeviceConfigComplete){
      return true;
    }
    return false;
  }

  void update(){
    promise.process();
  }

  void message(const char* event, const char* data){}

  void log(const char* data);

private:
  bool FLAG_locationChanged = false;
  bool FLAG_rainForecastComplete = false;
  bool FLAG_sunriseForecastComplete = false;
  bool FLAG_getDeviceConfigComplete = false;

  void getSunrise();
  void getDeviceConfig();
  void getRain();

  void HANDLER_getSunrise(const char*, const char*);
  void HANDLER_deviceConfig(const char*, const char*);

  int calcStartTime(int, int);
};
