#pragma once

#include "application.h"
#include "sprinkler-troll-util.h"
#include "config.h"
#include <ArduinoJson.h>
#include <PublishManager.h>

extern PublishManager publishManager;
extern Stats SprinklerStats;

class CloudManager {
public:
  CloudManager(){}

  void begin(){
    Particle.subscribe("hook-response/getSunrise", &CloudManager::getSunriseResponseHandler, this, MY_DEVICES);
    Particle.subscribe("hook-response/getGoogleDocs", &CloudManager::getGoogleDocsResponseHandler, this, MY_DEVICES);
    Particle.subscribe("hook-response/getRain24Hours", &CloudManager::getRain24HoursResHandler, this, MY_DEVICES);
  }

  void onConnect(){
    getSunriseTime(SprinklerStats.cityID);
    getRain24Hours(SprinklerStats.cityID);
    publishManager.publish("getGoogleDocs","null");
  }

  void update();

  void publishMessage(const char*,const char*);

  void publishStats(void);

private:

  uint32_t calcStartTime(uint32_t deadline, uint32_t duration);
  void getSunriseTime(const char* cityId);
  void getSunriseTime(uint32_t cityId);
  void getRain24Hours(const char* cityId);
  void getRain24Hours(uint32_t cityId);
  void getSunriseResponseHandler(const char *, const char *);
  void getGoogleDocsResponseHandler(const char *event, const char *data);
  void getRain24HoursResHandler(const char *, const char *);
};
