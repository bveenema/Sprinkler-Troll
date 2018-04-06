#include "application.h"
#include <ArduinoJson.h>

enum ShouldProcess {
  NO_PROCESS,
  PROCESS
};

void publishMessage(const char*,const char*,ShouldProcess=PROCESS);
void getSunriseTime(const char* cityId, ShouldProcess=PROCESS);
void getSunriseTime(uint32_t cityId, ShouldProcess=PROCESS);
