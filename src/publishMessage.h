#include "application.h"
#include <ArduinoJson.h>

enum ShouldProcess {
  NO_PROCESS,
  PROCESS
};

void publishMessage(const char*,const char*,ShouldProcess=PROCESS);
