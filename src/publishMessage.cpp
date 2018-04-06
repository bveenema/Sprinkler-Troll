#include "publishMessage.h"

void publishMessage(const char* destination, const char* message, ShouldProcess shouldProcess){
  const size_t bufferSize = JSON_OBJECT_SIZE(2)+80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["message"] = message;
  root["time"] = Time.now();

  char buffer[bufferSize];
  root.printTo(buffer);

  Particle.publish(destination, buffer, 60, PRIVATE);
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

  Particle.publish("getSunrise", buffer, 60, PRIVATE);
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
