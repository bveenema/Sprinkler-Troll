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
extern bool serialReady;

class CloudManager {
public:
  CloudManager(){}

  void begin(){
    promise.enable();
  }

  void onConnect(enum sprinkler_states state){
    // Get Sunrise Time
    if(state == SPRINKLER_OFF) {
      promise.create(&CloudManager::getSunriseOpenWeather, this, "sunrise-time-ow")
             .then(&CloudManager::sunriseOpenWeatherResponseHandler, this);
    }

    // Get Rain Forecast
    promise.create([]{
      char buffer[255];
      sprintf(buffer, "{\"state\":\"%s\",\"city\":\"%s\"}",SprinklerStats.state,SprinklerStats.city);
      publishManager.publish("getRainWunderground",buffer);
    }."rain-wgnd").then([](const char* event, const char* data){
      this->publishMessage("General", data);
    });

    // Get Device Configuration
    promise.create([]{
              publishManager.publish("getDeviceConfig","null")
            },"device-config")
            .then(&CloudManager::deviceConfigResponseHandler, this);
  }

  void update(){
    publishManager.process();
    promise.process();
  }

  void publishMessage(const char*,const char*);
  void publishStats(void);

private:
  uint32_t calcStartTime(uint32_t deadline, uint32_t duration);
  void getSunriseOpenWeather();
  void getRainOpenWeather(uint32_t cityId);
  void getRainWunderground(const char *state, const char *city);
  void sunriseOpenWeatherResponseHandler(const char *, const char *);
  void rainOpenWeatherResHandler(const char *, const char *);
  void rainWundergroundHandler(const char *, const char *);
  void deviceConfigResponseHandler(const char *event, const char *data);
};
