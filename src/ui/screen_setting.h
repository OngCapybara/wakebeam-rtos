#pragma once
#include <Arduino.h>

// mode: 0 = Set Jam, 1 = Set Alarm
// cursorField: 0 = Jam, 1 = Menit, 2 = Status ON/OFF (khusus alarm)
void renderSettingScreen(uint8_t mode, uint8_t editHour, uint8_t editMin, bool alarmOn, uint8_t cursorField);