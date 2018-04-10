#include "application.h"
#include "config.h"
#include <ArduinoJson.h>
#include <PublishManager.h>

extern PublishManager publishManager;

enum ShouldProcess {
  NO_PROCESS,
  PROCESS
};

void publishMessage(const char*,const char*,ShouldProcess=PROCESS);
void getSunriseTime(const char* cityId, ShouldProcess=PROCESS);
void getSunriseTime(uint32_t cityId, ShouldProcess=PROCESS);
void getSunriseResponseHandler(const char *, const char *);

extern Stats SprinklerStats;
