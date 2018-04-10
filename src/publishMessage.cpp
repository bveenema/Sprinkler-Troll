#include "publishMessage.h"

void publishMessage(const char* destination, const char* message, ShouldProcess shouldProcess){
  const size_t bufferSize = JSON_OBJECT_SIZE(2)+80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["message"] = message;
  root["time"] = Time.now();

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish(destination, buffer);
  if(shouldProcess == PROCESS){
    Particle.process();
  }
  return;
}

void getSunriseTime(const char* cityId, ShouldProcess shouldProcess){
  const size_t bufferSize = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["cityId"] = cityId; //5084633

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("getSunrise", buffer);
  if(shouldProcess == PROCESS){
    Particle.process();
  }
  return;
}

void getSunriseTime(uint32_t cityId, ShouldProcess shouldProcess){
  char sCityId[10];
  itoa(cityId, sCityId, 10);
  getSunriseTime(sCityId, shouldProcess);
  return;
}

void getSunriseResponseHandler(const char *event, const char *data) {
  int32_t newDeadline = atoi(data);

  if(newDeadline != SprinklerStats.deadline){
    // only log change if greater than 60 seconds
    if(newDeadline > (SprinklerStats.deadline + 60)){
      char buffer[100];
      sprintf(buffer, "new DEADLINE: %i", newDeadline);
      publishMessage("googleDocs", buffer, PROCESS);
    }
    SprinklerStats.deadline = newDeadline;
    SprinklerStats.targetStartTime = newDeadline - SprinklerStats.duration;
    EEPROM.put(statsAddr, SprinklerStats);
  }

  publishManager.publish("response",Time.timeStr(newDeadline));
}
