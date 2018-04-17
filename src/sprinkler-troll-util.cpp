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

uint32_t determineSleepTime(uint32_t timeNow, uint32_t startTime, uint32_t deadline){
  uint32_t timeToSleep;
  const uint32_t lengthOfDay = 86400;

  // calculate time to startTime and time to deadline
  uint32_t timeToStartTime = lengthOfDay - timeNow + startTime;
  if(timeToStartTime >= lengthOfDay){
    timeToStartTime -= lengthOfDay;
  }

  uint32_t timeToDeadline = lengthOfDay - timeNow + deadline;
  if(timeToDeadline >= lengthOfDay){
    timeToDeadline -= lengthOfDay;
  }

  // Get lesser of time to startTime and time to deadline
  timeToSleep = min(timeToStartTime, timeToDeadline);

  // return lesser of timeToSleep and sleepTime;
  return min(timeToSleep, sleepTime);
}
