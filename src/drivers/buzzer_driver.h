#pragma once
#include <Arduino.h>
#include "../config.h"

void initBuzzer();
void playToneHz(uint32_t freq, uint32_t durationMs);
void stopBuzzer();