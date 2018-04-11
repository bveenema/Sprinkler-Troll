#include "application.h"

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
