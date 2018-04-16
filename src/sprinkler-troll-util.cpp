#include "sprinkler-troll-util.h"

uint32_t beginningOfDay(int32_t time){
  struct tm *thisTime;
  thisTime = gmtime(&time);
  thisTime->tm_hour = 0;
  thisTime->tm_min = 0;
  thisTime->tm_sec = 0;

  return (uint32_t)mktime(thisTime);
}

uint32_t timeOfDay(int32_t time){
  uint32_t beginOfDay = beginningOfDay(time);

  return time - beginOfDay;
}

uint32_t determineSleepTime(uint32_t startTime, uint32_t deadline){
  uint32_t timeToSleep;

  uint32_t timeNow = timeOfDay(Time.now());

  if(deadline >= startTime){
    if(timeNow <= deadline)
  }

  int32_t timeToDeadline = SprinklerStats.deadline - timeNow;
  if(timeToDeadline > 0){
    if(timeToDeadline < sleepTime){
      timeToSleep = timeToDeadline;
    }
  }

  // if(timeNow - SprinklerStats.deadline < sleepTime){
  //
  // }
  return 0;
}
