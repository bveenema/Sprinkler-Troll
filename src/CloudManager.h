#pragma once

#include "application.h"
#include "sprinkler-troll-util.h"
#include "config.h"
#include <ArduinoJson.h>
#include <PublishManager.h>

extern PublishManager<> publishManager;
extern Stats SprinklerStats;
extern bool serialReady;

class CloudManager {
public:
  CloudManager(){}

  void begin(){
    Particle.subscribe(System.deviceID() + "/hook-response", &CloudManager::webhookHandler, this, MY_DEVICES);
  }

  void onConnect(enum sprinkler_states state){
    if(state == SPRINKLER_OFF) getSunriseOpenWeather(SprinklerStats.cityID);
    getRainWunderground(SprinklerStats.stateName,SprinklerStats.cityName);
    publishManager.publish("getDeviceConfig","null");
  }

  void update(){
    publishManager.process();
  }

  void publishMessage(const char*,const char*);

  void publishStats(void);

private:

  uint32_t calcStartTime(uint32_t deadline, uint32_t duration);
  void getSunriseOpenWeather(uint32_t cityId);
  void getRainOpenWeather(uint32_t cityId);
  void getRainWunderground(const char *state, const char *city);
  void webhookHandler(const char *, const char *);
  void sunriseOpenWeatherResponseHandler(const char *, const char *);
  void rainOpenWeatherResHandler(const char *, const char *);
  void rainWundergroundHandler(const char *, const char *);
  void deviceConfigResponseHandler(const char *event, const char *data);
};
