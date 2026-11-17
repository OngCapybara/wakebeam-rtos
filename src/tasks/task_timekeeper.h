#pragma once
#include "../app_types.h"

extern TimeData gCurrentTime;
extern AlarmSlot gMainAlarm;
extern volatile bool gAlarmTriggered;

void taskTimekeeper(void *pvParameters);