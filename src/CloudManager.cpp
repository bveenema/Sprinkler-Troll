#include "CloudManager"
void CloudManager::publishStats(){
  const size_t bufferSize = JSON_OBJECT_SIZE(5);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["version"] = SprinklerStats.version;
  root["duration"] = SprinklerStats.duration;
  root["deadline"] = SprinklerStats.deadline;
  root["targetStartTime"] = SprinklerStats.targetStartTime;
  root["cityID"] = SprinklerStats.cityID;

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("General", buffer);
  return;
}

void CloudManager::publishMessage(const char* destination, const char* message){
  const size_t bufferSize = JSON_OBJECT_SIZE(2)+80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["message"] = message;
  root["time"] = Time.now();

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish(destination, buffer);
  return;
}

void CloudManager::getSunriseTime(const char* cityId){
  const size_t bufferSize = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["cityId"] = cityId; //5084633

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("getSunrise", buffer);
  return;
}

void CloudManager::getSunriseTime(uint32_t cityId){
  char sCityId[10];
  itoa(cityId, sCityId, 10);
  getSunriseTime(sCityId);
  return;
}

void CloudManager::getSunriseResponseHandler(const char *event, const char *data) {
  uint32_t newDeadline = timeOfDay(atoi(data));

  if(newDeadline != SprinklerStats.deadline){
    // only log change if greater than 60 seconds
    if((newDeadline >= (SprinklerStats.deadline + 5)) || (newDeadline <= (SprinklerStats.deadline - 5))){
      char buffer[100];
      sprintf(buffer, "new DEADLINE: %u", newDeadline);
      this->publishMessage("googleDocs", buffer);
    }
    SprinklerStats.deadline = newDeadline;
    SprinklerStats.targetStartTime = newDeadline - SprinklerStats.duration;
    this->publishStats();
    EEPROM.put(statsAddr, SprinklerStats);
  }
}

void CloudManager::getGoogleDocsResponseHandler(const char *event, const char *data) {

  const size_t bufferSize = JSON_OBJECT_SIZE(4) + 80;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(data);

  uint32_t newDuration = root["duration"];
  uint32_t newCityID = root["cityID"];
  // const char* name = root["name"];
  // const char* coreid = root["coreid"];

  // Check that newCityID is 6 or 7 digit integer
  if(newCityID >= 100000 && newCityID <= 9999999){
    if(newCityID != SprinklerStats.cityID){
      char buffer[100];
      sprintf(buffer, "new City ID: %u", (unsigned int)newCityID);
      this->publishMessage("googleDocs", buffer);
      SprinklerStats.cityID = newCityID;
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID CITY ID: %u", (unsigned int)newCityID);
    this->publishMessage("googleDocs", buffer);
  }

  // Check that duration is valid number
  if(newDuration < maxDuration){
    if(newDuration != SprinklerStats.duration){
      char buffer[100];
      sprintf(buffer, "new DURATION: %u", (unsigned int)newDuration);
      this->publishMessage("googleDocs", buffer);
      SprinklerStats.duration = newDuration;
      this->publishStats();
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID DURATION: %u", (unsigned int)newDuration);
    this->publishMessage("googleDocs", buffer);
  }
}
