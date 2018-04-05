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
