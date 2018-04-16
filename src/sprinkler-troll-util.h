#pragma once

#include "application.h"
#include "config.h"


// Time Utilities
uint32_t beginningOfDay(int32_t);
uint32_t timeOfDay(int32_t);

// Sleep Utilities
uint32_t determineSleepTime(void);
