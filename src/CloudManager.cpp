#include "CloudManager.h"

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

void CloudManager::getSunriseOpenWeather(uint32_t cityId){
  char sCityId[10];
  itoa(cityId, sCityId, 10);

  const size_t bufferSize = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["cityId"] = sCityId;

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("getSunriseOpenWeather", buffer);
  return;
}

void CloudManager::getRainOpenWeather(uint32_t cityId){
  char sCityId[10];
  itoa(cityId, sCityId, 10);

  const size_t bufferSize = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["cityId"] = sCityId;

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("getRain24Hours", buffer);
  return;
}

void CloudManager::getRainWunderground(const char* state, const char* city){
  char buffer[255];
  sprintf(buffer, "{\"state\":\"%s\",\"city\":\"%s\"}",state,city);
  publishManager.publish("getRainWunderground",buffer);
}

void CloudManager::webhookHandler(const char *event, const char *data) {
  if(strstr(event, "getSunriseOpenWeather")){
    this->sunriseOpenWeatherResponseHandler(event, data);
  }
  if(strstr(event, "getDeviceConfig")){
    this->deviceConfigResponseHandler(event, data);
  }
  if(strstr(event, "getRainWunderground")){
    this->publishMessage("General", data);
  }
}

void CloudManager::sunriseOpenWeatherResponseHandler(const char *event, const char *data) {

  char dataStore[255];
  strcpy(dataStore, data);

  uint32_t newDeadline = timeOfDay(atoi(dataStore));

  if(newDeadline != SprinklerStats.deadline){
    // only log change if greater than 5 seconds
    if((newDeadline >= (SprinklerStats.deadline + 5)) || (newDeadline <= (SprinklerStats.deadline - 5))){
      char buffer[100];
      sprintf(buffer, "new DEADLINE: %u", (unsigned int)newDeadline);
      this->publishMessage("googleDocs", buffer);
    }
    SprinklerStats.deadline = newDeadline;
    SprinklerStats.targetStartTime = this->calcStartTime(SprinklerStats.deadline,SprinklerStats.duration);
    this->publishStats();
    EEPROM.put(statsAddr, SprinklerStats);
  }
}

void CloudManager::deviceConfigResponseHandler(const char *event, const char *data) {

  char dataStore[255];
  strcpy(dataStore, data);

  const size_t bufferSize = JSON_OBJECT_SIZE(4) + 80;
  StaticJsonBuffer<bufferSize> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(dataStore);

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
      this->getSunriseOpenWeather(SprinklerStats.cityID);
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID CITY ID: %u", (unsigned int)newCityID);
    this->publishMessage("googleDocs", buffer);
  }

  // Check that duration is valid number
  if(newDuration <= maxDuration){
    if(newDuration != SprinklerStats.duration){
      char buffer[100];
      sprintf(buffer, "new DURATION: %u", (unsigned int)newDuration);
      this->publishMessage("googleDocs", buffer);
      SprinklerStats.duration = newDuration;
      SprinklerStats.targetStartTime = this->calcStartTime(SprinklerStats.deadline,SprinklerStats.duration);
      this->publishStats();
      EEPROM.put(statsAddr, SprinklerStats);
    }
  } else {
    char buffer[100];
    sprintf(buffer, "INVALID DURATION: %u", (unsigned int)newDuration);
    this->publishMessage("googleDocs", buffer);
  }
}

void CloudManager::rainOpenWeatherResHandler(const char *event, const char *data){

  char dataStore[255];
  strcpy(dataStore, data);

  const size_t bufferSize = JSON_ARRAY_SIZE(8) + 80;
  StaticJsonBuffer<bufferSize> jsonBuffer;

  JsonArray& root = jsonBuffer.parseArray(dataStore);

  JsonArray& root_ = root;
  float root_0 = root_[0]; // 0.69092
  float root_1 = root_[1]; // 0.69092
  float root_2 = root_[2]; // 0.69092
  float root_3 = root_[3]; // 0.6909
  float sum = 0;
  for(uint8_t i=0; i<8; i++){
    float root = root_[i];
    sum = sum + root;
  }
  this->publishMessage("Rain Response", dataStore);
  this->publishMessage("Rain Response", String(sum).c_str());
}

uint32_t CloudManager::calcStartTime(uint32_t deadline, uint32_t duration){
  uint32_t lenOfDay = 86400; // length of one day in seconds (ignore leap seconds)
  if(duration <= deadline){
    return deadline - duration;
  } else {
    return lenOfDay + (deadline - duration);
  }
}
