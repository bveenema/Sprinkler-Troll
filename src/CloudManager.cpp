#include "CloudManager.h"

void cloudManager::onConnect(enum sprinkler_states state){
  getDeviceConfig();
  getRain();
  if(state == SPRINKLER_OFF) getSunrise();
}

void cloudManager::publishStats(){
  const size_t bufferSize = JSON_OBJECT_SIZE(7)+40;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["version"] = SprinklerStats.version;
  root["duration"] = SprinklerStats.duration;
  root["deadline"] = SprinklerStats.deadline;
  root["targetStartTime"] = SprinklerStats.targetStartTime;
  root["city"] = SprinklerStats.cityName;
  root["state"] = SprinklerStats.stateName;
  root["timezone"] = SprinklerStats.timeZone;

  char buffer[bufferSize];
  root.printTo(buffer);

  publishManager.publish("General", buffer);
  return;
}

void cloudManager::getDeviceConfig(){
  promise.create([]{
            publishManager.publish("getDeviceConfig","null");
          }, "device-config")
          .then(&cloudManager::HANDLER_deviceConfig,this)
          .finally([this]{
             FLAG_getDeviceConfigComplete = true;
             if(!FLAG_locationChanged){
               promise.cancel("rain");
               promise.cancel("sunrise");
               FLAG_rainForecastComplete = false;
               FLAG_sunriseForecastComplete = false;
               getSunrise();
               getRain();
             }
           });
}

void cloudManager::getSunrise(){
  promise.create([this]{
            char buffer[255];
            sprintf(buffer, "{\"state\":\"%s\",\"city\":\"%s\"}",SprinklerStats.stateName,SprinklerStats.cityName);
            publishManager.publish("getSunriseWunderground",buffer);
          }, "sunrise")
         .then(&cloudManager::HANDLER_getSunrise, this)
         .finally([this]{
           FLAG_sunriseForecastComplete = true;
         });
}

void cloudManager::getRain(){
  promise.create([this]{
            char buffer[255];
            sprintf(buffer, "{\"state\":\"%s\",\"city\":\"%s\"}",SprinklerStats.stateName,SprinklerStats.cityName);
            publishManager.publish("getRainWunderground",buffer);
          }, "rain")
          .then([this](const char* event, const char* data){
            this->publishMessage("General", data);
          })
          .finally([this]{
            FLAG_rainForecastComplete = true;
          });
}

void cloudManager::HANDLER_deviceConfig(const char *event, const char *data) {

  char dataStore[255];
  strcpy(dataStore, data);

  const size_t bufferSize = JSON_OBJECT_SIZE(6) + 120;
  StaticJsonBuffer<bufferSize> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(dataStore);

  int newDuration = root["duration"];
  const char* newStateName = root["stateName"];
  const char* newCityName = root["cityName"];
  // const char* name = root["name"];
  // const char* coreid = root["coreid"];

  if(strcmp(newStateName, SprinklerStats.stateName) != 0 ||
     strcmp(newCityName, SprinklerStats.cityName) != 0      )
  {
       FLAG_locationChanged = true;
  }

  if(FLAG_locationChanged){
    char buffer[255];
    sprintf(buffer, "new Location: %s, %s", newStateName, newCityName);
    this->publishMessage("googleDocs", buffer);
    strcpy(SprinklerStats.stateName, newStateName);
    strcpy(SprinklerStats.cityName, newCityName);
    EEPROM.put(statsAddr, SprinklerStats);
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

void cloudManager::HANDLER_getSunrise(const char *event, const char *data){
  // Interpret Data
  char dataStore[255];
  strcpy(dataStore, data);

  const size_t bufferSize = JSON_OBJECT_SIZE(2);
  StaticJsonBuffer<bufferSize> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(dataStore);

  int hour = atoi(root["hour"]);
  int minute = atoi(root["minute"]);
  int newDeadline = hour*3600 + minute*60 - SprinklerStats.timeZone*3600;

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

int cloudManager::calcStartTime(int deadline, int duration){
  uint32_t lenOfDay = 86400; // length of one day in seconds (ignore leap seconds)
  if(duration <= deadline){
    return deadline - duration;
  } else {
    return lenOfDay + (deadline - duration);
  }
}
