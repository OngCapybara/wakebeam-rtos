#pragma once
#include <Arduino.h>

extern volatile int8_t gOverrideFaceStyle; // -1 jika tidak ada override dari HP

void taskBle(void *pvParameters);